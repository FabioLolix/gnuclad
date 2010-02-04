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
      << "  width='" << width << "'\n"
      << "  height='" << height << "'\n"
      << ">\n\n";


  //////////////////////////////////////////////////////////////////////////////
  // Definitions

  *fp << "<defs>\n\n"

  // Rulers
      << "  <line id='__ruler' x1='0' y1='0' x2='0' y2='100%' stroke='#" << clad->rulerColor.hex << "' stroke-width='" << clad->rulerWidth << "'/>\n\n"
      << "  <line id='__rulerMonth' x1='0' y1='0' x2='0' y2='100%' stroke='#" << clad->rulerMonthColor.hex << "' stroke-width='" << clad->rulerMonthWidth << "'/>\n\n"

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


  // Add domain gradients
  for(int i = 0; i < (int)clad->domains.size(); ++i) {
    Domain * d = clad->domains.at(i);
    *fp << "  <linearGradient id='__domain_" << validxml(d->nodeName) << "' x1='0' y1='0' x2='1' y2='0'>\n"
        << "    <stop stop-color='#" << d->color.hex << "' offset='0' stop-opacity='0' />\n"
        << "    <stop stop-color='#" << d->color.hex << "' offset='1' stop-opacity='" << float(d->intensity) / 100 << "' />\n"
        << "  </linearGradient>\n";
  }


  // Add connector markers
  *fp << "\n  <circle id='__connectors_start' cx='0' cy='0' r='" << lPX << "' stroke='none' />\n";
  for(int i = 0; i < (int)clad->connectors.size(); ++i) {
    *fp << "  <marker id='__connector_" << i << "' stroke='none' markerUnits='userSpaceOnUse' style='overflow:visible;'>\n"
        << "    <use xlink:href='#__connectors_start' fill='#" << clad->connectors.at(i)->color.hex << "'  />\n"
        << "  </marker>\n";
  }


  // Add stop gradients
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

  // Additional SVG images - definitions
  Image * image;
  for(int i = 0; i < (int)clad->includeSVG.size(); ++i) {
    image = clad->includeSVG.at(i);

    ifstream inf( (image->filename).c_str() );
    if( !inf.is_open() ) throw "failed to open SVG image " + image->filename;

    *fp << "\n\n";
    string line;
    while( !inf.eof() && inf.good() ) {

      getline(inf, line);
      if( line.find("<defs") != string::npos ) {  // if we get to the defs
        while(line.find(">") == string::npos) getline(inf, line); // closing tag
        getline(inf, line); // closing tag
        while(line.find("</defs>") == string::npos) {  // as long as within defs
          *fp << line << "\n";
          getline(inf, line);
        }
        break;
      }

    }

    *fp << "\n\n";
    inf.close();
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
    int x = i * yrPX + xPX;
    *fp  << "  <use xlink:href='#__ruler' x='" << x << "' />\n";
    for(int j = 1; j < clad->monthsInYear && i < years; ++j)
      *fp << "  <use xlink:href='#__rulerMonth' x='" << x + j * yrPX / clad->monthsInYear << "' />\n";
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
      else *fp << datePX(n->parent->start, clad) + xPX << " " << posYparent << " L ";

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


  // Labels
  *fp << "\n<g inkscape:label='Labels' inkscape:groupmode='layer' id='layer_labels'\n"
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
    string alignment;

    if(clad->labelBackground == 1)
      *fp << "  <rect x='" << posX - dirty_hack_em/4 << "' y='" << posY - dirty_hack_ex *6/5 << "' width='" << n->name.size() * dirty_hack_em
          << "' height='" << dirty_hack_ex *7/5 << "' fill='#" << clad->mainBackground.hex << "' opacity='0.5'  rx='5' ry='5' />\n";

    *fp << "  " << href << "<text x='"<< posX <<"' y='"<< posY <<"' >" << n->name <<"</text>" << hrefend << "\n";

    for(int j = 0; j < (int)n->nameChanges.size(); ++j) {

      if(clad->nameChangeType == 0) {

        posX = datePX(n->nameChanges.at(j).date, clad) + xPX + clad->smallDotRadius;
        if(posX < posXwName) posX = posXwName;
        if(j != 0) posXwName = posX + n->nameChanges.at(j-1).newName.size() * dirty_hack_em;

      } else if(clad->nameChangeType == 1) {

        posX = datePX(n->nameChanges.at(j).date, clad) + xPX;
        posY = n->offset * oPX + yrlinePX + dirty_hack_ex/2;
        alignment = "style='text-anchor:middle;'";
        clad->labelBackground = 0;                                              // <= is adjusting the label background pointless?

      }

      if(clad->descriptionIsHyperLink == 1)
        href = "<a xlink:href='" + n->nameChanges.at(j).description + "'>";

      if(clad->labelBackground == 1)
        *fp << "    <rect x='" << posX - dirty_hack_em/4 << "' y='" << posY - dirty_hack_ex *6/5 << "' width='" << n->nameChanges.at(j).newName.size() * dirty_hack_em
            << "' height='" << dirty_hack_ex *7/5 << "' fill='#" << clad->mainBackground.hex << "' opacity='0.5'  rx='5' ry='5' />\n";

      *fp << "    " << href << "<text x='"<< posX <<"' y='"<< posY <<"' " << alignment << ">" << n->nameChanges.at(j).newName <<"</text>" << hrefend << "\n";

    }

  }
  *fp << "</g>\n";


  // Year Lines with the year numbers
  *fp << "\n<g inkscape:label='Yearlines' inkscape:groupmode='layer' id='layer_yearlines'\n"
      << " style='stroke:none;fill:url(#__yearline);' >\n";
  yrlinePX -= 3 * oPX / 2;  // remove small margin
  dirty_hack_ex = int(clad->yearLineFontSize / 1.375 * clad->fontCorrectionFactor);  // CSS ex unit
  if(yrlinePX > 0) {

    *fp << "  <rect x='0' y='0' rx='5' ry='5' width='" << width << "' height='" << yrlinePX << "' />\n"
        << "  <rect x='0' y='" << height - yrlinePX << "' rx='5' ry='5' width='" << width << "' height='" << yrlinePX << "' />\n"
        << "  <g style='font-size:" << clad->yearLineFontSize << "px;stroke:none;fill:#" << clad->yearLineFontColor.hex << ";font-family:" << clad->yearLineFont << ";-inkscape-font-specification:" << clad->yearLineFont << ";text-anchor:middle;' >\n";
    for(int i = 0; i < years; ++i) {
      int posX = yrPX * i + yrPX / 2 + xPX;
      int posY = yrlinePX / 2 + 0.5 * dirty_hack_ex;
      *fp << "    <text x='" << posX << "' y='" << posY << "'><tspan>" << clad->beginningOfTime.year + i << "</tspan></text>\n"
          << "    <text x='" << posX << "' y='" << height - posY + dirty_hack_ex << "'><tspan>" << clad->beginningOfTime.year + i << "</tspan></text>\n";
    }
    *fp << "  </g>\n";

  }
  *fp << "</g>\n";


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


  // Additional SVG images
  *fp << "\n<g inkscape:label='Included SVG Images' inkscape:groupmode='layer' id='layer_included_svg'>\n";
  for(int i = 0; i < (int)clad->includeSVG.size(); ++i) {

    image = clad->includeSVG.at(i);

    ifstream inf( (image->filename).c_str() );
    if( !inf.is_open() ) throw "failed to open SVG image " + image->filename;
    *fp << "  <g transform='translate(" << image->x << "," << image->y << ")' >\n";

    string line;
    while( !inf.eof() && inf.good() ) {

      getline(inf, line);
      if( line.find("</defs>") != string::npos ) {  // if we get behind the defs
        getline(inf, line);
        while(line.find("</svg>") == string::npos) {  // as long as witihn svg
          *fp << line << "\n";
          getline(inf, line);
        }
        break;
      }

    }

    *fp << "  </g>\n";
    inf.close();

  }
  *fp << "</g>\n";



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
