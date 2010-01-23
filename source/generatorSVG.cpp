/*
*  generatorSVG.cpp - implements a SVG generator for gnuclad
*
*  Copyright (C) 2010 Donjan Rodic <drodic@student.ethz.ch>
*
*  This program is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "generatorSVG.h"

using namespace std;


GeneratorSVG::GeneratorSVG() {
}

void GeneratorSVG::writeData(Cladogram * clad, ofstream * fp) {

  int xPX = 10;
  int yrPX = clad->yearPX;
  int oPX = clad->offsetPX;
  int lPX = clad->lineWidth;
  int yrlinePX = clad->yearLinePX + 3 * oPX / 2;  // add small margin

  int years = clad->endOfTime.year - clad->beginningOfTime.year + 1;

  int width = years * yrPX + 2 * xPX;
  int height = clad->maximumOffset * oPX + 2 * yrlinePX;
// if(orientation == vertical) swap(width, height); // or transform the whole svg with rotation transform???

  //////////////////////////////////////////////////////////////////////////////
  // Header (Inkscape compatible)

  *fp << "<?xml version='1.0' encoding='UTF-8' standalone='no'?>\n"
      << "<!-- Created with gnuclad -->\n"
      << "\n"
      << "<svg\n"
      << "  xmlns='http://www.w3.org/2000/svg'\n"
      << "  xmlns:svg='http://www.w3.org/2000/svg'\n"
      << "  xmlns:xlink='http://www.w3.org/1999/xlink'\n"
      << "  xmlns:inkscape='http://www.inkscape.org/namespaces/inkscape'\n"  // for Inkscape layers
      << "  version='1.1'\n"
      << "  width='" << width << "'\n"
      << "  height='" << height << "'\n"
      << ">\n\n";

  *fp << "<defs>\n\n"
      << "  <line id='ruler' x1='0' y1='0' x2='0' y2='100%' stroke='#" << clad->rulerColor.hex << "' stroke-width='" << clad->rulerWidth << "'/>\n\n"
      << "  <line id='rulerMonth' x1='0' y1='0' x2='0' y2='100%' stroke='#" << clad->rulerMonthColor.hex << "' stroke-width='" << clad->rulerMonthWidth << "'/>\n\n"
      << "  <linearGradient id='__yearline' x1='0' y1='0' x2='1' y2='0' stroke='none'>\n"
      << "    <stop stop-color='#" << clad->yearLineColor1.hex << "' offset='0' stop-opacity='1' />\n"
      << "    <stop stop-color='#" << clad->yearLineColor2.hex << "' offset='1' stop-opacity='1' />\n"
      << "  </linearGradient>\n\n"
      << "  <filter id='__label_background' color-interpolation-filters='sRGB'>\n"
      << "    <feGaussianBlur stdDeviation='5' />\n"
      << "  </filter>\n\n";

  // Infobox properties
  *fp << "  <linearGradient id='__infobox_fill' x1='0' y1='0' x2='0' y2='1' stroke='none'>\n"
      << "    <stop stop-color='#" << clad->infoBoxColor1.hex << "' offset='0' stop-opacity='1' />\n"
      << "    <stop stop-color='#" << clad->infoBoxColor2.hex << "' offset='1' stop-opacity='1' />\n"
      << "  </linearGradient>\n"
      << "  <linearGradient id='__infobox_stroke' x1='0' y1='0' x2='0' y2='1' stroke='none'>\n"
      << "    <stop stop-color='#" << clad->infoBoxColor2.hex << "' offset='0' stop-opacity='1' />\n"
      << "    <stop stop-color='#" << clad->infoBoxColor1.hex << "' offset='1' stop-opacity='1' />\n"
      << "  </linearGradient>\n"
      << "  <filter id='__infobox_shadow' color-interpolation-filters='sRGB'>\n"
      << "    <feGaussianBlur stdDeviation='5' />\n"
      << "  </filter>\n\n";

  // Add domain gradients
  for(int i = 0; i < (int)clad->domains.size(); ++i) {
    Domain * d = clad->domains.at(i);
    *fp << "  <linearGradient id='__domain_" << ws2underscore(d->nodeName) << "' x1='0' y1='0' x2='1' y2='0' stroke='none'>\n"
        << "    <stop stop-color='#" << d->color.hex << "' offset='0' stop-opacity='0' />\n"
        << "    <stop stop-color='#" << d->color.hex << "' offset='1' stop-opacity='" << float(d->intensity) / 100 << "' />\n"
        << "  </linearGradient>\n";
  }

  *fp << "\n";

  // Add stop gradients
  Node * n;
  for(int i = 0; i < (int)clad->nodes.size(); ++i) {

    n = clad->nodes.at(i);
    if(n->stop < clad->endOfTime) {
      int fade = clad->stopFadeOutPX / lPX;
      string name = ws2underscore(n->name);
      *fp << "  <linearGradient id='__fadeout_" << name << "' x1='0' y1='0' x2='" << fade << "' y2='0' gradientUnits='userSpaceOnUse' stroke='none'>\n"
          << "    <stop stop-color='#" << n->color.hex << "' offset='0' stop-opacity='1' />\n"
          << "    <stop stop-color='#" << n->color.hex << "' offset='1' stop-opacity='0' />\n"
          << "  </linearGradient>\n"
          << "  <marker id='__stop_" << name << "' markerWidth='" << fade << "' markerHeight='1' style='overflow:visible'>\n"
          << "    <line x1='0' y1='0' x2='" << fade << "' y2='0' stroke='url(#__fadeout_" << name << ")' stroke-width='1'/>\n"
          << "  </marker>\n";
    }

  }

  *fp << "\n</defs>\n";


  //////////////////////////////////////////////////////////////////////////////
  // Content

  // Background
  *fp << "\n<g inkscape:label='Background' inkscape:groupmode='layer' id='layer_background' >\n"
      << "  <rect x='0' y='0' width='" << width << "' height='" << height << "' fill='#" << clad->mainBackground.hex << "' />\n"
      << "</g>\n";

  // Year and month Rulers
  *fp << "\n<g inkscape:label='Year Rulers' inkscape:groupmode='layer' id='layer_yearrulers'>\n";
  for(int i = 0; i <= years; ++i) {
    *fp  << "  <use xlink:href='#ruler' x='" << i * yrPX + xPX << "' />\n";
    for(int j = 1; j < clad->monthsInYear; ++j)
      *fp << "  <use xlink:href='#rulerMonth' x='" << i * yrPX + xPX + j * yrPX / clad->monthsInYear<< "' />\n";
  }
  *fp << "</g>\n";

  // Domains
  *fp << "\n<g inkscape:label='Domains' inkscape:groupmode='layer' id='layer_domains' >\n";
  for(int i = 0; i < (int)clad->domains.size(); ++i) {

    Domain * d = clad->domains.at(i);
    int xpos = datePX(d->node->start, clad) + xPX;
    int yval = (d->offsetA - 1) * oPX + yrlinePX;
    int hval = (d->offsetB - d->offsetA + 2) * oPX;
    *fp << "  <rect x='" << xpos << "' y='" << yval << "' width='" << width - xpos - xPX << "' height='" << hval
        << "' rx='" << oPX / 2 << "' ry='" << oPX / 2 << "' fill='url(#__domain_" << ws2underscore(d->nodeName) << ")' />\n";

  }

  *fp << "</g>\n";
  // Connectors
  Connector * c;
  *fp << "\n<g inkscape:label='Connectors' inkscape:groupmode='layer' id='layer_connectors' >\n";
  for(int i = 0; i < (int)clad->connectors.size(); ++i) {

    c = clad->connectors.at(i);
    int posX = datePX(c->when, clad) + xPX;
    int posY1 = c->offsetA * oPX + yrlinePX;
    int posY2 = c->offsetB * oPX + yrlinePX;
    string dash = "";
    if(clad->connectorsDashed == 1) dash = "stroke-dasharray='" + int2str(c->thickness) + "," + int2str(c->thickness) + "'";
    *fp << "  <line x1='" << posX << "' y1='" << posY1 << "' x2='" << posX << "' y2='" << posY2
        << "' stroke='#" << c->color.hex << "' stroke-width='" << c->thickness << "' " << dash << " />\n";
  }
  *fp << "</g>\n";

  // Node Lines
  *fp << "\n<g inkscape:label='Lines' inkscape:groupmode='layer' id='layer_lines'\n"
      << " style='fill:none;stroke-width:" << lPX << ";' >\n";
  for(int i = 0; i < (int)clad->nodes.size(); ++i) {

    n = clad->nodes.at(i);
    int startX = datePX(n->start, clad) + xPX;
    int stopX = datePX(n->stop, clad) + xPX;
    int posY = n->offset * oPX + yrlinePX;
    *fp << "  <path id='line_"<< ws2underscore(n->name) <<"' d='M ";
    if(n->parent != NULL) {
      if(clad->derivType == 0) *fp << startX << " " << n->parent->offset * oPX + yrlinePX + lPX/2 << " L ";
      else *fp << datePX(n->parent->start, clad) + xPX << " " << n->parent->offset * oPX + yrlinePX + lPX/2 << " L ";
    }
    *fp << startX << " " << posY << " L " << stopX << " " << posY
        << "' stroke='#"<< n->color.hex <<"'";
    if(n->stop < clad->endOfTime)
      *fp << " marker-end='url(#__stop_" << ws2underscore(n->name) << ")'";
    *fp << " />\n";

  }
  *fp << "</g>\n";

  // Dots
  *fp << "\n<g inkscape:label='Dots' inkscape:groupmode='layer' id='layer_dots'\n"
      << " style='stroke:none;' >\n";
  for(int i = 0; i < (int)clad->nodes.size(); ++i) {

    n = clad->nodes.at(i);
    int posX = datePX(n->start, clad) + xPX;
    int posY = n->offset * oPX + yrlinePX;
    *fp << "  <circle id='dot_" << ws2underscore(n->name) << "' cx='" << posX << "' cy='" << posY
        << "' r='" << clad->dotRadius << "' fill='#" << n->color.hex << "' stroke='none' />\n";

    for(int j = 0; j < (int)n->nameChanges.size(); ++j) {
      posX = datePX(n->nameChanges.at(j).date, clad) + xPX;
      *fp << "    <circle cx='" << posX << "' cy='" << posY << "' r='" << clad->smallDotRadius << "' fill='#" << n->color.hex << "' stroke='none' />\n";
    }

  }
  *fp << "</g>\n";

  // Labels
  *fp << "\n<g inkscape:label='Labels' inkscape:groupmode='layer' id='layer_labels' xlink:type='simple'\n"
      << " style='font-size:" << clad->labelFontSize << "px;stroke:none;fill:#" << clad->labelFontColor.hex << ";font-family:" << clad->labelFont << ";-inkscape-font-specification:" << clad->labelFont << ";' >\n";
  int dirty_hack_em = int(clad->labelFontSize / 1.675);
  int dirty_hack_ex = int(clad->labelFontSize / 1.675);
  for(int i = 0; i < (int)clad->nodes.size(); ++i) {
    n = clad->nodes.at(i);
    string href = "", hrefend = "";
    if(clad->descriptionIsHyperLink == 1) {
      href = "<a xlink:href='" + n->description + "'>";
      hrefend = "</a>";
    }
    int posX = datePX(n->start, clad) + xPX + clad->dotRadius;
    int posY = n->offset * oPX + yrlinePX - (oPX/5);  // last value is experimental
    int posXwName = posX + n->name.size() * dirty_hack_em;
    if(clad->labelBackground == 1)
      *fp << "  <rect x='" << posX - dirty_hack_em/3 << "' y='" << posY - dirty_hack_ex *4/3 << "' width='" << n->name.size() * dirty_hack_em << "' height='" << dirty_hack_ex *5/3 << "' fill='#" << clad->mainBackground.hex << "' opacity='0.8'  rx='5' ry='5' />\n";//filter='url(#__label_background)' />\n";
    *fp << "  " << href << "<text x='"<< posX <<"' y='"<< posY <<"' >" << n->name <<"</text>" << hrefend << "\n";
    for(int j = 0; j < (int)n->nameChanges.size(); ++j) {
      if(j != 0) posXwName = posX + n->nameChanges.at(j-1).newName.size() * dirty_hack_em;
      posX = datePX(n->nameChanges.at(j).date, clad) + xPX + clad->smallDotRadius;
      if(posX < posXwName) posX = posXwName;
      *fp << "    <text x='"<< posX <<"' y='"<< posY <<"' >" << n->nameChanges.at(j).newName <<"</text>\n";
    }

  }
  *fp << "</g>\n";

  // Year Lines with the year numbers
  *fp << "\n<g inkscape:label='Yearlines' inkscape:groupmode='layer' id='layer_yearlines'\n"
      << " style='stroke:none;fill:url(#__yearline);' >\n";
  yrlinePX -= 3 * oPX / 2;  // remove small margin
  dirty_hack_ex = int(clad->yearLineFontSize / 1.675);  // CSS ex unit Inkscape is supposed to support but doesn't
  if(yrlinePX > 0) {

    *fp << "  <rect x='0' y='0' rx='5' ry='5' width='" << width << "' height='" << yrlinePX << "' />\n"
        << "  <rect x='0' y='" << height - yrlinePX << "' rx='5' ry='5' width='" << width << "' height='" << yrlinePX << "' />\n"
        << "  <g style='font-size:" << clad->yearLineFontSize << "px;stroke:none;fill:#" << clad->yearLineFontColor.hex << ";font-family:" << clad->yearLineFont << ";-inkscape-font-specification:" << clad->yearLineFont << ";text-anchor:middle;' >\n";
    for(int i = 0; i < years; ++i)
      *fp << "    <text x='" << yrPX * i + yrPX / 2 + xPX << "' y='" << yrlinePX  / 2 << "'><tspan dy='" << 0.5 * dirty_hack_ex<< "'>" << clad->beginningOfTime.year + i << "</tspan></text>\n"
          << "    <text x='" << yrPX * i + yrPX / 2 + xPX << "' y='" << height - yrlinePX / 2 << "'><tspan dy='" << 0.5 * dirty_hack_ex<< "'>" << clad->beginningOfTime.year + i << "</tspan></text>\n";
    *fp << "  </g>\n";

  }
  *fp << "</g>\n";

  // The infobox
  dirty_hack_ex = int(clad->infoBoxTitleSize / 1.675);
  int dirty_hack_ex2 = int(clad->infoBoxTextSize / 1.675);
  vector<string> textv;
  explode(clad->infoBoxText, '|', &textv);
  *fp << "\n<g inkscape:label='Infobox' inkscape:groupmode='layer' id='layer_infobox'\n"
      << " style='stroke:none;font-family:" << clad->yearLineFont << ";-inkscape-font-specification:" << clad->yearLineFont << ";' >\n"
      << "  <rect x='" << clad->infoBoxX << "' y='" << clad->infoBoxY << "' width='" << clad->infoBoxWidth << "' height='" << clad->infoBoxHeight << "' rx='10' ry='10' filter='url(#__infobox_shadow)' opacity='0.6' />\n"
      << "  <rect x='" << clad->infoBoxX << "' y='" << clad->infoBoxY << "' width='" << clad->infoBoxWidth << "' height='" << clad->infoBoxHeight << "' rx='10' ry='10' fill='url(#__infobox_fill)' stroke='url(#__infobox_stroke)' />\n"
      << "  <text x='" << clad->infoBoxX + dirty_hack_ex * 2/3 << "' y='" << clad->infoBoxY + dirty_hack_ex *2 << "'><tspan style='font-size:" << clad->infoBoxTitleSize << "px;'>" << clad->infoBoxTitle << "</tspan></text>\n"
      << "  <text x='" << clad->infoBoxX + dirty_hack_ex * 2/3 << "' y='" << clad->infoBoxY + dirty_hack_ex *2 + 10 << "' style='font-size:" << clad->infoBoxTextSize << "px;'>\n";
  for(int i = 0; i < (int)textv.size(); ++i)
    *fp << "    <tspan x='" << clad->infoBoxX + dirty_hack_ex * 2/3 << "' dy='" << dirty_hack_ex2 * 2 << "'>" << textv.at(i) << "</tspan>\n";
  *fp << "  </text>\n"
      << "</g>\n";



  *fp << "\n</svg>\n";

}


string ws2underscore(string str) {
  for(int i = 0; i < (int)str.size(); ++i)
    if(str[i] == ' ') str[i] = '_';
  return str;
}
