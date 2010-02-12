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


GeneratorSVG::GeneratorSVG() {}
GeneratorSVG::~GeneratorSVG() {}

void GeneratorSVG::writeData(Cladogram * clad, ofstream * fp) {

  int xPX = 10;
  int yrPX = clad->yearPX;
  int oPX = clad->offsetPX;
  int lPX = clad->lineWidth;
  int yrlinePX = clad->yearLinePX + 3 * oPX / 2;  // add small margin

  int years = clad->endOfTime.year - clad->beginningOfTime.year + 1;

  int width = years * yrPX + 2 * xPX;
  int height = clad->maximumOffset * oPX + 2 * yrlinePX;
  int canvasWidth = width;
  int canvasHeight = height;
  if(clad->orientation == 1 || clad->orientation == 3)
    swap(canvasWidth, canvasHeight);

  //////////////////////////////////////////////////////////////////////////////
  // Header (Inkscape compatible)

  *fp << "<?xml version='1.0' encoding='UTF-8' standalone='yes'?>\n"
      << "<!-- Created with gnuclad -->\n"
      << "\n"
      << "<svg\n"
      << "  xmlns='http://www.w3.org/2000/svg'\n"
      << "  xmlns:svg='http://www.w3.org/2000/svg'\n"
      << "  xmlns:xlink='http://www.w3.org/1999/xlink'\n"
      << "  xmlns:inkscape='http://www.inkscape.org/namespaces/inkscape'\n"  // for Inkscape layers
      << "  xmlns:sodipodi='http://sodipodi.sourceforge.net/DTD/sodipodi-0.dtd'\n"  // and Sodipodi compatibility
      << "  xmlns:dc='http://purl.org/dc/elements/1.1/'\n"
      << "  xmlns:cc='http://creativecommons.org/ns#'\n"
      << "  xmlns:rdf='http://www.w3.org/1999/02/22-rdf-syntax-ns#'\n"
      << "  version='1.1'\n"
      << "  width='" << canvasWidth << "'\n"
      << "  height='" << canvasHeight << "'\n"
      << ">\n\n";


  //////////////////////////////////////////////////////////////////////////////
  // Definitions

  *fp << "<defs>\n\n"

  // Yearline gradient
      << "  <linearGradient id='__yearline' x1='0' y1='0' x2='1' y2='0'>\n"
      << "    <stop stop-color='#" << clad->yearLineColor1.hex << "' offset='0' stop-opacity='1' />\n"
      << "    <stop stop-color='#" << clad->yearLineColor2.hex << "' offset='1' stop-opacity='1' />\n"
      << "  </linearGradient>\n\n";


  // Infobox properties
  *fp << "  <linearGradient id='__infobox_fill' x1='0' y1='0' x2='0' y2='1'>\n"
      << "    <stop stop-color='#" << clad->infoBoxColor1.hex << "' offset='0' stop-opacity='1' />\n"
      << "    <stop stop-color='#" << clad->infoBoxColor2.hex << "' offset='1' stop-opacity='1' />\n"
      << "  </linearGradient>\n"
      << "  <linearGradient id='__infobox_stroke' x1='0' y1='0' x2='0' y2='1'>\n"
      << "    <stop stop-color='#" << clad->infoBoxColor2.hex << "' offset='0' stop-opacity='1' />\n"
      << "    <stop stop-color='#" << clad->infoBoxColor1.hex << "' offset='1' stop-opacity='1' />\n"
      << "  </linearGradient>\n"
      << "  <filter id='__infobox_shadow' color-interpolation-filters='sRGB'>\n"
      << "    <feGaussianBlur stdDeviation='5' />\n"
      << "  </filter>\n\n";


  // Domain gradients
  for(int i = 0; i < (int)clad->domains.size(); ++i) {
    Domain * d = clad->domains.at(i);
    *fp << "  <linearGradient id='__domain_" << validxml(d->nodeName) << "' x1='0' y1='0' x2='1' y2='0'>\n"
        << "    <stop stop-color='#" << d->color.hex << "' offset='0' stop-opacity='0' />\n"
        << "    <stop stop-color='#" << d->color.hex << "' offset='1' stop-opacity='" << float(d->intensity) / 100 << "' />\n"
        << "  </linearGradient>\n";
  }


  // Connector markers
  *fp << "\n  <circle id='__connectors_start' cx='0' cy='0' r='" << lPX << "' stroke='none' />\n";
  for(int i = 0; i < (int)clad->connectors.size(); ++i) {
    *fp << "  <marker id='__connector_" << i << "' stroke='none' markerUnits='userSpaceOnUse' style='overflow:visible;'>\n"
        << "    <use xlink:href='#__connectors_start' fill='#" << clad->connectors.at(i)->color.hex << "'  />\n"
        << "  </marker>\n";
  }


  // Stop gradients
  Node * n;
  int fade = clad->stopFadeOutPX / lPX;
  *fp << "\n  <line id='__fadeout' x1='0' y1='0' x2='" << fade << "' y2='0' stroke-width='1' />\n";
  for(int i = 0; i < (int)clad->nodes.size(); ++i) {

    n = clad->nodes.at(i);
    if(n->stop < clad->endOfTime) {
      string name = validxml(n->name);
      *fp << "  <linearGradient id='__fadeout_" << name << "' x1='0' y1='0' x2='" << fade << "' y2='0' gradientUnits='userSpaceOnUse'>\n"
          << "    <stop stop-color='#" << n->color.hex << "' offset='0' stop-opacity='1' />\n"
          << "    <stop stop-color='#" << n->color.hex << "' offset='1' stop-opacity='0' />\n"
          << "  </linearGradient>\n"
          << "  <marker id='__stop_" << name << "' markerWidth='" << fade << "' markerHeight='1' style='overflow:visible;'>\n"
          << "    <use xlink:href='#__fadeout' stroke='url(#__fadeout_" << name << ")' />\n"
          << "  </marker>\n";
    }

  }


  // Icons - definitions for SVG files
  for(int i = 0; i < (int)clad->nodes.size(); ++i)
    if(getExt(clad->nodes.at(i)->iconfile) == "svg")
      *fp << SVG_defs(clad->nodes.at(i)->iconfile);


  // Additional SVG images - definitions
  *fp << "\n<!-- BEGIN additional SVG images - definitions -->\n";
  for(int i = 0; i < (int)clad->includeSVG.size(); ++i)
    *fp << SVG_defs(clad->includeSVG.at(i)->filename);
  *fp << "\n<!-- END additional SVG images - definitions -->\n";

  *fp << "\n</defs>\n";


  //////////////////////////////////////////////////////////////////////////////
  // Content


  // Orientation START
  string transform = "", retransformLabels = "", retransformYearlines = "";
  if     (clad->orientation == 0) {}
  else if(clad->orientation == 1) {
    transform = "transform='matrix(0,1,1,0,0,1)'";
    retransformLabels = "transform='matrix(0,1,1,0,0,1)'";
    retransformYearlines = "transform='matrix(0,1,1,0,0,1) rotate(90,0,0) translate(0,-" + int2str(canvasWidth) + ")'";
  }
  else if(clad->orientation == 2) {
    transform = "transform='matrix(-1,0,0,1,0,1) translate(-" + int2str(canvasWidth) + ",0)'";
    retransformLabels  = "transform='matrix(-1,0,0,1,0,1) translate(-" + int2str(canvasWidth) + ",0)'";
    retransformYearlines  = "transform='matrix(-1,0,0,1,0,1) translate(-" + int2str(canvasWidth) + ",0)'";
  }
  else if(clad->orientation == 3) {
    transform = "transform='rotate(-90,0,0) translate(-" + int2str(canvasHeight) + ",0)'";
    retransformLabels  = "transform='translate(" + int2str(canvasHeight) + ",0) rotate(90,0,0)'";
  }
  if(clad->orientation != 0)
    *fp << "\n<g id='orientation' " << transform << "><!-- BEGIN orientation transform -->\n";


  // Background
  *fp << "\n<g inkscape:label='Background' inkscape:groupmode='layer' id='layer_background' >\n"
      << "  <rect x='0' y='0' width='" << width << "' height='" << height << "' fill='#" << clad->mainBackground.hex << "' />\n"
      << "</g>\n";


  // Year and month Rulers
  *fp << "\n<g inkscape:label='Year Rulers' inkscape:groupmode='layer' id='layer_yearrulers'"
      << " stroke-width='" << clad->rulerMonthWidth << "' stroke='#" <<  clad->rulerMonthColor.hex  << "'>\n";
  for(int i = 0; i <= years; ++i) {
    int x = i * yrPX + xPX;
    int xm;
    *fp << "  <line x1='" << x << "' y1='0' x2='" << x << "' y2='" << height << "'"
        << " stroke-width='" << clad->rulerWidth << "' stroke='#" <<  clad->rulerColor.hex  << "' />\n";
    for(int j = 1; j < clad->monthsInYear && i < years; ++j) {
      xm = x + j * yrPX / clad->monthsInYear;
      *fp << "    <line x1='" << xm << "' y1='0' x2='" << xm << "' y2='" << height << "' />\n";
    }

  }
  *fp << "</g>\n";


  // Domains
  *fp << "\n<g inkscape:label='Domains' inkscape:groupmode='layer' id='layer_domains' >\n";
  for(int i = 0; i < (int)clad->domains.size(); ++i) {

    Domain * d = clad->domains.at(i);
    int xpos = datePX(d->node->start, clad) + xPX;
    int yval = (d->offsetA - 1) * oPX + yrlinePX;
    int hval = (d->offsetB - d->offsetA + 2) * oPX;
    *fp << "  <rect x='" << xpos << "' y='" << yval << "' width='" << width - (xpos + xPX) << "' height='" << hval
        << "' rx='" << oPX / 2 << "' ry='" << oPX / 2 << "' fill='url(#__domain_" << validxml(d->nodeName) << ")' />\n";

  }

  *fp << "</g>\n";


  // Connectors
  Connector * c;
  *fp << "\n<g inkscape:label='Connectors' inkscape:groupmode='layer' id='layer_connectors'\n"
      <<  "style='opacity:0.6;' >\n";
  for(int i = 0; i < (int)clad->connectors.size(); ++i) {

    c = clad->connectors.at(i);
    int posX1 = datePX(c->fromWhen, clad) + xPX;
    int posX2 = datePX(c->toWhen, clad) + xPX;
    int posY1 = c->offsetA * oPX + yrlinePX;
    int posY2 = c->offsetB * oPX + yrlinePX;
    int sign;
    if(c->from->offset < c->to->offset) sign = 1;
    else sign = -1;
    string connectorDot = "";
    if(clad->connectorDots == 1) connectorDot = " marker-start='url(#__connector_" + int2str(i) + ")'";
    string dash = "";
    if(clad->connectorsDashed == 1) dash = "stroke-dasharray='" + int2str(c->thickness) + "," + int2str(c->thickness) + "'";
    *fp << "  <line x1='" << posX1 << "' y1='" << posY1 + sign * lPX/2 << "' x2='" << posX2 << "' y2='" << posY2
        << "' stroke='#" << c->color.hex << "' stroke-width='" << c->thickness << "' " << dash << connectorDot << " />\n";
  }
  *fp << "</g>\n";


  // Node Lines
  *fp << "\n<g inkscape:label='Lines' inkscape:groupmode='layer' id='layer_lines'\n"
      << " style='fill:none;stroke-width:" << lPX << ";' >\n";
  for(int i = 0; i < (int)clad->nodes.size(); ++i) {

    n = clad->nodes.at(i);
    int sign;
    int startX = datePX(n->start, clad) + xPX;
    int stopX = datePX(n->stop, clad) + xPX;
    int posY = n->offset * oPX + yrlinePX;
    *fp << "  <path id='__line_"<< validxml(n->name) <<"' d='M ";
    if(n->parent != NULL) {

      if(n->offset < n->parent->offset) sign = 1;
      else sign = -1;
      int posYparent = n->parent->offset * oPX + yrlinePX - sign * lPX/2;

      if(clad->derivType == 0) *fp << startX << " " << posYparent << " L ";
      else if(clad->derivType == 1) *fp << datePX(n->parent->start, clad) + xPX << " " << posYparent << " L ";

    }
    *fp << startX << " " << posY << " L " << stopX << " " << posY
        << "' stroke='#"<< n->color.hex <<"'";
    if(n->stop < clad->endOfTime)
      *fp << " marker-end='url(#__stop_" << validxml(n->name) << ")'";
    *fp << " />\n";

  }
  *fp << "</g>\n";


  // Dots
  string style;
  if     (clad->dotType == 0) style = "stroke:none;";
  else if(clad->dotType == 1) style = "stroke-width:" + int2str(clad->lineWidth / 2) + ";fill:#" + clad->mainBackground.hex + ";";
  *fp << "\n<g inkscape:label='Dots' inkscape:groupmode='layer' id='layer_dots'\n"
      << " style='" << style << "' >\n";
  for(int i = 0; i < (int)clad->nodes.size(); ++i) {

    n = clad->nodes.at(i);
    int posX = datePX(n->start, clad) + xPX;
    int posY = n->offset * oPX + yrlinePX;
    string dotprops;
    if     (clad->dotType == 0) dotprops = "fill='#" + n->color.hex + "' stroke='none'";
    else if(clad->dotType == 1) dotprops = "stroke='#" + n->color.hex + "'";

    *fp << "  <circle id='__dot_" << validxml(n->name) << "' cx='" << posX << "' cy='" << posY
        << "' r='" << clad->dotRadius << "' " << dotprops << " />\n";

    for(int j = 0; j < (int)n->nameChanges.size(); ++j) {
      posX = datePX(n->nameChanges.at(j).date, clad) + xPX;
      *fp << "    <circle cx='" << posX << "' cy='" << posY << "' r='" << clad->smallDotRadius << "' " << dotprops << " />\n";
    }

  }
  *fp << "</g>\n";


  // Icons
  *fp << "\n<g inkscape:label='Icons' inkscape:groupmode='layer' id='layer_icons'>\n";
  string iconfile, format;
  for(int i = 0; i < (int)clad->nodes.size(); ++i) {

    n = clad->nodes.at(i);
    iconfile = n->iconfile;
    format = getExt(iconfile);

    if(format == "") continue;
    else if(format == "svg") {

      int iconWidth = 0;
      int iconHeight = 0;

      string icon = SVG_body(iconfile, iconWidth, iconHeight);

      int posX = datePX(n->start, clad) + xPX - iconWidth/2;
      int posY = n->offset * oPX + yrlinePX - iconHeight/2;

      *fp << "  <g transform='translate(" << posX << "," << posY << ")' >\n"
          << icon
          << "  </g>\n";

    } else if(format == "png") {

      int iconWidth = 0;
      int iconHeight = 0;

      string icon = base64_png(iconfile, iconWidth, iconHeight);

      int posX = datePX(n->start, clad) + xPX - iconWidth/2;
      int posY = n->offset * oPX + yrlinePX - iconHeight/2;

      *fp << "  <image id='__icon_" << validxml(n->name) << "' x='" << posX << "' y='" << posY << "' width='" << iconWidth << "' height='" << iconHeight << "'\n"
          << "    xlink:href='data:image/" << format << ";base64," << icon << "' />\n";

    } else throw "unknown icon file format: " + format +
                 "\n       accepted formats: svg, png";

  }
  *fp << "</g>\n";


  // Labels
  *fp << "\n<g inkscape:label='Labels' inkscape:groupmode='layer' id='layer_labels' " << retransformLabels << "\n"
      << " style='font-size:" << clad->labelFontSize << "px;stroke:none;fill:#" << clad->labelFontColor.hex << ";font-family:"
      << clad->labelFont << ";-inkscape-font-specification:" << clad->labelFont << ";' >\n";
  int dirty_hack_em = int(clad->labelFontSize / 1.675 * clad->fontCorrectionFactor);
  int dirty_hack_ex = int(clad->labelFontSize / 1.375 * clad->fontCorrectionFactor);
  for(int i = 0; i < (int)clad->nodes.size(); ++i) {

    n = clad->nodes.at(i);
    string href = "", hrefend = "";

    if(clad->descriptionIsHyperLink == 1) {
      href = "<a xlink:href='" + n->description + "'>";
      hrefend = "</a>";
    }

    int posX = datePX(n->start, clad) + xPX + clad->dotRadius;
    int posY = n->offset * oPX + yrlinePX - dirty_hack_ex/2;  // last value is experimental
    int posXwName = posX + n->name.size() * dirty_hack_em;
    int alignmentBGx = posX - dirty_hack_em/4;
    string alignment = "";

    if(clad->orientation == 1) {

      posX = n->offset * oPX + yrlinePX;
      posY = datePX(n->start, clad) + xPX - clad->dotRadius - dirty_hack_ex/5;

      alignment = "style='text-anchor:middle;'";
      alignmentBGx = posX - n->name.size() * dirty_hack_em / 2;

    } else if(clad->orientation == 2) {

      posX = width - posX - n->name.size() * dirty_hack_em;
      alignmentBGx = width - alignmentBGx - n->name.size() * dirty_hack_em;

    } else if(clad->orientation == 3) {

      posX = n->offset * oPX + yrlinePX;
      posY = canvasHeight - (datePX(n->start, clad) + xPX - clad->dotRadius - dirty_hack_ex * 7/5);

      alignment = "style='text-anchor:middle;'";
      alignmentBGx = posX - n->name.size() * dirty_hack_em / 2;

    }

    if(clad->labelBGOpacity > 0)
      *fp << "  <rect x='" << alignmentBGx << "' y='" << posY - dirty_hack_ex *6/5 << "' width='" << n->name.size() * dirty_hack_em
          << "' height='" << dirty_hack_ex *7/5 << "' fill='#" << clad->mainBackground.hex << "' opacity='" << double(clad->labelBGOpacity)/100 
          << "'  rx='5' ry='5' />\n";

    *fp << "  " << href << "<text x='"<< posX <<"' y='"<< posY <<"' " << alignment << " >" << n->name <<"</text>" << hrefend << "\n";


    string alignmentNameChange;
    for(int j = 0; j < (int)n->nameChanges.size(); ++j) {

      if(clad->nameChangeType == 0) {  // names outside the dot

        posX = datePX(n->nameChanges.at(j).date, clad) + xPX + clad->smallDotRadius;
        if(posX < posXwName) posX = posXwName;
        if(j != 0) posXwName = posX + n->nameChanges.at(j-1).newName.size() * dirty_hack_em;

      } else if(clad->nameChangeType == 1) {  // names centered on the dot

        posX = datePX(n->nameChanges.at(j).date, clad) + xPX;
        posY = n->offset * oPX + yrlinePX + dirty_hack_ex/2;
        alignmentNameChange = "style='text-anchor:middle;'";

      }

      if(clad->orientation == 1) {

        posX = n->offset * oPX + yrlinePX + clad->smallDotRadius;
        posY = datePX(n->nameChanges.at(j).date, clad) + xPX - clad->smallDotRadius;
        if(clad->nameChangeType == 1) {
          posX -= clad->smallDotRadius;
          posY += clad->smallDotRadius + dirty_hack_ex/2;
        }

      } else if(clad->orientation == 2) {

        posX = width - posX - n->nameChanges.at(j).newName.size() * dirty_hack_em;

      } else if(clad->orientation == 3) {

        posX = n->offset * oPX + yrlinePX + clad->smallDotRadius;
        posY = canvasHeight - (datePX(n->nameChanges.at(j).date, clad) + xPX + clad->smallDotRadius/2);
        if(clad->nameChangeType == 1) {
          posX -= clad->smallDotRadius;
          posY += clad->smallDotRadius/2 + dirty_hack_ex/2;
        }

      }

      if(clad->descriptionIsHyperLink == 1)
        href = "<a xlink:href='" + n->nameChanges.at(j).description + "'>";

      if(clad->labelBGOpacity > 0 && clad->nameChangeType != 1)
        *fp << "    <rect x='" << posX - dirty_hack_em/4 << "' y='" << posY - dirty_hack_ex *6/5 << "' width='" << n->nameChanges.at(j).newName.size() * dirty_hack_em
            << "' height='" << dirty_hack_ex *7/5 << "' fill='#" << clad->mainBackground.hex << "' opacity='" << double(clad->labelBGOpacity)/100 << "'  rx='5' ry='5' />\n";

      *fp << "    " << href << "<text x='"<< posX <<"' y='"<< posY <<"' " << alignmentNameChange << ">" << n->nameChanges.at(j).newName <<"</text>" << hrefend << "\n";

    }

  }
  *fp << "</g>\n";


  // Year Lines with the year numbers
  *fp << "\n<g inkscape:label='Yearlines' inkscape:groupmode='layer' id='layer_yearlines' " << retransformYearlines << "\n"
      << " style='stroke:none;fill:url(#__yearline);' >\n";
  yrlinePX -= 3 * oPX / 2;  // remove small margin
  dirty_hack_ex = int(clad->yearLineFontSize / 1.375 * clad->fontCorrectionFactor);  // CSS ex unit
  if(yrlinePX > 0) {

    *fp << "  <rect x='0' y='0' rx='5' ry='5' width='" << width << "' height='" << yrlinePX << "' />\n"
        << "  <rect x='0' y='" << height - yrlinePX << "' rx='5' ry='5' width='" << width << "' height='" << yrlinePX << "' />\n"
        << "  <g style='font-size:" << clad->yearLineFontSize << "px;stroke:none;fill:#" << clad->yearLineFontColor.hex << ";font-family:" << clad->yearLineFont << ";-inkscape-font-specification:" << clad->yearLineFont << ";text-anchor:middle;' >\n";
    for(int i = 0; i < years; ++i) {
      int posX = yrPX * i + yrPX / 2 + xPX;
      int posY = yrlinePX / 2 + dirty_hack_ex / 2;
      int yeartext = clad->beginningOfTime.year + i;
      if(clad->orientation == 2) yeartext = clad->endOfTime.year -i;
      *fp << "    <text x='" << posX << "' y='" << posY << "'><tspan>" << yeartext << "</tspan></text>\n"
          << "    <text x='" << posX << "' y='" << height - posY + dirty_hack_ex << "'><tspan>" << yeartext << "</tspan></text>\n";
    }
    *fp << "  </g>\n";

  }
  *fp << "</g>\n";


  // Orientation STOP
  if(clad->orientation != 0)
    *fp << "</g><!-- END orientation transform -->\n";


  // The infobox
  dirty_hack_ex = int(clad->infoBoxTitleSize / 1.375 * clad->fontCorrectionFactor);
  int dirty_hack_ex2 = int(clad->infoBoxTextSize / 1.375 * clad->fontCorrectionFactor * 1.8);
  int posX = clad->infoBoxX + dirty_hack_ex * 1/2;
  int posY = clad->infoBoxY + dirty_hack_ex *5/3;
  *fp << "\n<g inkscape:label='Infobox' inkscape:groupmode='layer' id='layer_infobox'\n"
      << " style='stroke:none;fill:#" << clad->infoBoxFontColor.hex << ";font-family:" << clad->infoBoxFont << ";-inkscape-font-specification:" << clad->infoBoxFont << ";' >\n"
      << "  <rect x='" << clad->infoBoxX << "' y='" << clad->infoBoxY << "' width='" << clad->infoBoxWidth << "' height='" << clad->infoBoxHeight << "' rx='10' ry='10' fill='#000' filter='url(#__infobox_shadow)' opacity='0.6' />\n"
      << "  <rect x='" << clad->infoBoxX << "' y='" << clad->infoBoxY << "' width='" << clad->infoBoxWidth << "' height='" << clad->infoBoxHeight << "' rx='10' ry='10' fill='url(#__infobox_fill)' stroke='url(#__infobox_stroke)' stroke-width='2' />\n";
  if(clad->infoBoxTitleSize > 0)
    *fp << "  <text x='" << posX << "' y='" << posY << "'><tspan style='font-size:" << clad->infoBoxTitleSize << "px;font-weight:bold;'>" << clad->infoBoxTitle << "</tspan></text>\n";
  if(clad->infoBoxTextSize > 0 )
    for(int i = 0; i < (int)clad->infoBoxText.size(); ++i)
      *fp << "  <text x='" << posX << "' y='" << posY + dirty_hack_ex + dirty_hack_ex2 * i << "'><tspan style='font-size:" << clad->infoBoxTextSize << "px;'>" << clad->infoBoxText.at(i) << "</tspan></text>\n";
  *fp << "</g>\n";

  *fp << "\n<!-- BEGIN additional images -->\n";


  // Additional PNG images
  Image * image;
  *fp << "\n<g inkscape:label='Included PNG Images' inkscape:groupmode='layer' id='layer_included_png'>\n";
  for(int i = 0; i < (int)clad->includePNG.size(); ++i) {
    image = clad->includePNG.at(i);

    int imgWidth = 0;
    int imgHeight = 0;
    string data = base64_png(image->filename, imgWidth, imgHeight);

    *fp << "  <image id='__png_" << int2str(i) << "' x='" << image->x << "' y='" << image->y << "' width='" << imgWidth << "' height='" << imgHeight << "'\n"
        << "    xlink:href='data:image/png;base64," << data << "' />\n";
  }
  *fp << "</g>\n";


  // Additional SVG images
  *fp << "\n<g inkscape:label='Included SVG Images' inkscape:groupmode='layer' id='layer_included_svg'>\n";
  for(int i = 0; i < (int)clad->includeSVG.size(); ++i) {

    int void1, void2;
    image = clad->includeSVG.at(i);
    string data = SVG_body(image->filename, void1, void2);
    *fp << "  <g transform='translate(" << image->x << "," << image->y << ")' >\n"
        << data
        << "  </g>\n";

  }
  *fp << "</g>\n";

  *fp << "\n<!-- END additional images -->\n";


  *fp << "\n</svg>\n";

}


string validxml(string str) {
  // We could simply encode in base64, but then the output file would be
  // much less human readable, which is bad.
  // Hence pushing the chars into "allowed space".
  for(int i = 0; i < (int)str.size(); ++i) {
    unsigned char c = (unsigned char)str[i];

    if(c >= 127) c -= 128;  // remove above 127
    if(c >= 123) c -= 5;  // push down to 'z'
    if(c <= 32) c += 33;  // remove unprintable
    while(c <= 47) c += 10; // push up to '0'

    if(c >= 58 && c <= 64) c -= 10;  // remove :;<=>=@
    if(c >= 91 && c <= 96) c -= 6;  // remove ^_`

    str[i] = (char)c;
  }
  return str;
}


// Returns the definitions of the specified SVG image (within <defs></defs)
std::string SVG_defs(std::string filename) {
  ifstream fp( filename.c_str() );
  if( !fp.is_open() ) throw "failed to open SVG image " + filename;

  string line, data = "";
  while( !fp.eof() && fp.good() ) {

    getline(fp, line);
    if( line.find("<defs") != string::npos ) {  // if we get to the defs
      while(line.find(">") == string::npos) getline(fp, line); // closing tag
      getline(fp, line); // closing tag
      while(line.find("</defs>") == string::npos) {  // as long as within defs
        data += line + "\n";
        getline(fp, line);
      }
      break;
    }

  }
  fp.close();
  return data;
}

// Returns the body of the specified SVG image (after </defs>, up to </svg>)
// Also sets the supplied width and height parameters to the correct value
std::string SVG_body(std::string filename, int &width, int &height) {
  ifstream fp( filename.c_str() );
  if( !fp.is_open() ) throw "failed to open SVG image " + filename;

  string line, data = "";
  while( !fp.eof() && fp.good() ) {

    // set width and height
    getline(fp, line);
    if( line.find("<svg") != string::npos ) {
      size_t pos;
      while(line.find(">") == string::npos) {
        if( (pos = line.find("width")) != string::npos) {
          string tmp = line.substr(pos + 5);
          tmp = tmp.substr(tmp.find_first_of("\"'") + 1);
          tmp = tmp.substr(0, tmp.find_first_of("\"'"));
          width = str2int(tmp);
        }
        if( (pos = line.find("height")) != string::npos) {
          string tmp = line.substr(pos + 6);
          tmp = tmp.substr(tmp.find_first_of("\"'") + 1);
          tmp = tmp.substr(0, tmp.find_first_of("\"'"));
          height = str2int(tmp);
        }
        getline(fp, line);
      }
    }

    // get contents
    if( line.find("</defs>") != string::npos ) {  // if we get behind the defs
      getline(fp, line);
      while(line.find("</svg>") == string::npos) {  // as long as within svg
        data += line + "\n";
        getline(fp, line);
      }
      break;
    }

  }
  fp.close();
  return data;
}


// Read a specified PNG file and return it as a base64 string
// Also sets the supplied width and height parameters to the correct value
std::string base64_png(std::string filename, int &width, int &height) {

    ifstream fp(filename.c_str(), ios::in|ios::binary|ios::ate);
    if( !fp.is_open() ) throw "failed to open PNG image " + filename;

    int size = fp.tellg();
    char * data = new char [size];
    fp.seekg (0);
    fp.read(data, size);

    if(data[1] != 'P' || data[2] != 'N' || data[3] != 'G')
      throw "invalid PNG file: " + filename;

    string data64 = base64_encode(data, size);

    // get width/height out of PNG's big endian format
    width = ((int)data[16] << 12) | ((int)data[17] << 8) |
                ((int)data[18] << 4)  |  (int)data[19];
    height = ((int)data[20] << 12) | ((int)data[21] << 8) |
                 ((int)data[22] << 4)  |  (int)data[23];

    delete[] data;
    fp.close();

    return data64;
}
