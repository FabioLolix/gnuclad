/*
*  svg.cpp - implements a SVG generator for gnuclad
*
*  Copyright (C) 2010-2013 Donjan Rodic <donjan@dyx.ch>
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

#include <cmath>
#include "svg.h"

using namespace std;


GeneratorSVG::GeneratorSVG() {}
GeneratorSVG::~GeneratorSVG() {}

void GeneratorSVG::writeData(Cladogram * clad, OutputFile & out) {

  ofstream & f = *(out.p);

  int xPX = 10;
  int yrPX = clad->yearPX;
  int oPX = clad->offsetPX;
  int lPX = clad->lineWidth;
  int yrlinePX = clad->yearLinePX + 3 * oPX / 2;  // add small margin
  int topOffset = yrlinePX;

  int years = clad->endOfTime.year - clad->beginningOfTime.year + 1;
  years += clad->appendYears;
  years += clad->prependYears;

  int width = years * yrPX + 2 * xPX;
  int height = clad->maximumOffset * oPX + 2 * yrlinePX;
  xPX += clad->prependYears * yrPX;

  // enum orientation left-to-right, top-to-bottom, right-to-left, bottom-to-top
  enum { oLR, oTB, oRL, oBT };

  // enum name change type
  enum { nc_outside, nc_inside};

  if(clad->infoBoxX < 0) {
    if(clad->orientation != oLR)
      throw "out of boundary infoBox only supported with orientation == 0";
    xPX -= clad->infoBoxX + 10;
    width -= clad->infoBoxX + 10;
    clad->infoBoxX = 10;
  }
  if(clad->infoBoxY < 0) {
    if(clad->orientation != oLR)
      throw "out of boundary infoBox only supported with orientation == 0";
    topOffset -= clad->infoBoxY + 10;
    height -= clad->infoBoxY + 10;
    clad->infoBoxY = 10;
  }

  int canvasWidth = width;
  int canvasHeight = height;
  if(clad->orientation == oTB || clad->orientation == oBT)
    swap(canvasWidth, canvasHeight);

  // Small helpers
  dirty_hack_em = int(clad->labelFontSize / 1.625 * clad->fontCorrectionFactor);
  dirty_hack_ex = int(clad->labelFontSize / 1.375 * clad->fontCorrectionFactor);



  //////////////////////////////////////////////////////////////////////////////
  // Header (Inkscape compatible)

  f << "<?xml version='1.0' encoding='UTF-8' standalone='yes'?>\n"
    << "<!-- Created with gnuclad " << clad->gnuclad_version << " -->\n"
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
    << ">\n\n"
    << "<title>" << clad->infoBoxTitle << "</title>"
    << "\n\n";

  //////////////////////////////////////////////////////////////////////////////
  // Definitions

  f << "<defs>\n\n"

  // Yearline gradient
    << "  <linearGradient id='__yearline' x1='0' y1='0' x2='1' y2='0'>\n"
    << "    <stop stop-color='#" << clad->yearLineColor1.hex << "' offset='0' stop-opacity='1' />\n"
    << "    <stop stop-color='#" << clad->yearLineColor2.hex << "' offset='1' stop-opacity='1' />\n"
    << "  </linearGradient>\n\n";


  // Infobox properties
  f << "  <linearGradient id='__infobox_fill' x1='0' y1='0' x2='0' y2='1'>\n"
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
    Domain * d = clad->domains[i];
    f << "  <linearGradient id='__domain_" << validxml(d->nodeName, true) << "' x1='0' y1='0' x2='1' y2='0'>\n"
      << "    <stop stop-color='#" << d->color.hex << "' offset='0' stop-opacity='0' />\n"
      << "    <stop stop-color='#" << d->color.hex << "' offset='1' stop-opacity='" << float(d->intensity) / 100 << "' />\n"
      << "  </linearGradient>\n";
  }


  // Connector markers
  f << "\n  <circle id='__connectors_start' cx='0' cy='0' r='" << lPX << "' stroke='none' />\n";
  for(int i = 0; i < (int)clad->connectors.size(); ++i) {
    f << "  <marker id='__connector_" << i << "' stroke='none' markerUnits='userSpaceOnUse' style='overflow:visible;'>\n"
      << "    <use xlink:href='#__connectors_start' fill='#" << clad->connectors[i]->color.hex << "'  />\n"
      << "  </marker>\n";
  }


  // Stop gradients
  Node * n;
  int fade = clad->stopFadeOutPX / lPX;

  f << "<rect id='__fadeout' height='1' width='" << fade << "' y='-0.5' x='0'/>";
  //  f << "\n  <line id='__fadeout' x1='0' y1='0' x2='" << fade << "' y2='0' stroke-width='1' />\n";  // line doesn't render in WebKit
  for(int i = 0; i < (int)clad->nodes.size(); ++i) {

    if(clad->stopFadeOutPX == 0) break;
    n = clad->nodes[i];
    if(n->stop < clad->endOfTime) {
      string name = validxml(n->name, true);
      f << "  <linearGradient id='__fadeout_" << name << "' x1='0' y1='0' x2='" << fade / (1 + (sqrt(n->size)-1) * clad->bigParent) << "' y2='0' gradientUnits='userSpaceOnUse'>\n"
        << "    <stop stop-color='#" << n->color.hex << "' offset='0' stop-opacity='1' />\n"
        << "    <stop stop-color='#" << n->color.hex << "' offset='1' stop-opacity='0' />\n"
        << "  </linearGradient>\n"
        << "  <marker id='__stop_" << name << "' markerWidth='" << fade / (1 + (sqrt(n->size)-1) * clad->bigParent) << "' markerHeight='1' style='overflow:visible;'>\n"
        << "    <use xlink:href='#__fadeout' style='fill:url(#__fadeout_" << name << ")' />\n"
        << "  </marker>\n";
    }
  }


  // Icons - definitions for SVG files
  for(int i = 0; i < (int)clad->nodes.size(); ++i)
    if(getExt(clad->nodes[i]->iconfile) == "svg")
      f << SVG_defs(clad->nodes[i]->iconfile);


  // Additional SVG images - definitions
  f << "\n<!-- BEGIN additional SVG images - definitions -->\n";
  for(int i = 0; i < (int)clad->includeSVG.size(); ++i)
    f << SVG_defs(clad->includeSVG[i]->filename);
  f << "\n<!-- END additional SVG images - definitions -->\n";

  f << "\n</defs>\n";



  //////////////////////////////////////////////////////////////////////////////
  // Content


  // Orientation START
  string transform = "", retransformLabels = "", retransformYearlines = "";
  if     (clad->orientation == oLR) {}
  else if(clad->orientation == oTB) {
    transform = "transform='matrix(0,1,1,0,0,1)'";
    retransformLabels = "transform='matrix(0,1,1,0,0,1)'";
    retransformYearlines = "transform='matrix(0,1,1,0,0,1) rotate(90,0,0) translate(0,-" + int2str(canvasWidth) + ")'";
  }
  else if(clad->orientation == oRL) {
    transform = "transform='matrix(-1,0,0,1,0,1) translate(-" + int2str(canvasWidth) + ",0)'";
    retransformLabels  = "transform='matrix(-1,0,0,1,0,1) translate(-" + int2str(canvasWidth) + ",0)'";
    retransformYearlines  = "transform='matrix(-1,0,0,1,0,1) translate(-" + int2str(canvasWidth) + ",0)'";
  }
  else if(clad->orientation == oBT) {
    transform = "transform='rotate(-90,0,0) translate(-" + int2str(canvasHeight) + ",0)'";
    retransformLabels  = "transform='translate(" + int2str(canvasHeight) + ",0) rotate(90,0,0)'";
  }
  if(clad->orientation != oLR)
    f << "\n<g id='orientation' " << transform << "><!-- BEGIN orientation transform -->\n";


  // Background
  f << "\n<g inkscape:label='Background' inkscape:groupmode='layer' id='layer_background' >\n"
    << "  <rect x='0' y='0' width='" << width << "' height='" << height << "'"
    << " rx='" << oPX / 2 << "' ry='" << oPX / 2 << "' fill='#" << clad->mainBackground.hex << "' />\n"
    << "</g>\n";


  // Year and month Rulers
  f << "\n<g inkscape:label='Year Rulers' inkscape:groupmode='layer' id='layer_yearrulers'"
    << " stroke-width='" << clad->rulerMonthWidth << "' stroke='#" <<  clad->rulerMonthColor.hex  << "'>\n";
  for(int i = 0; i <= years; ++i) {
    int x = i * yrPX + xPX - clad->prependYears*yrPX;
    int xm;
    int rulerWidth = (i % clad->yearLabelInterval == 0) ? (clad->rulerWidth) : (clad->rulerUnlabeledYearWidth);
    f << "  <line x1='" << x << "' y1='" << topOffset - yrlinePX << "' x2='" << x << "' y2='" << height << "'"
      << " stroke-width='" << rulerWidth << "' stroke='#" <<  clad->rulerColor.hex  << "' />\n";
    for(int j = 1; j < clad->monthsInYear && i < years; ++j) {
      xm = x + j * yrPX / clad->monthsInYear;
      f << "    <line x1='" << xm << "' y1='" << topOffset - yrlinePX << "' x2='" << xm << "' y2='" << height << "' />\n";
    }

  }
  f << "</g>\n";


  // Domains
  f << "\n<g inkscape:label='Domains' inkscape:groupmode='layer' id='layer_domains' >\n";
  for(int i = 0; i < (int)clad->domains.size(); ++i) {

    Domain * d = clad->domains[i];
    int xpos = datePX(d->node->start, clad) + xPX;
    int wval = datePX(clad->endOfTime.year + 1 + clad->appendYears, clad) - xpos + xPX;
    int yval = (d->offsetA - 1) * oPX + topOffset;
    int hval = (d->offsetB - d->offsetA + 2) * oPX;
    if(clad->tighterDomains == true) {
      yval += oPX/2;
      hval -= oPX;
    }
    f << "  <rect x='" << xpos << "' y='" << yval << "' width='" << wval << "' height='" << hval
      << "' rx='" << oPX / 2 << "' ry='" << oPX / 2 << "' fill='url(#__domain_" << validxml(d->nodeName, true) << ")' />\n";

  }

  f << "</g>\n";


  // Connectors
  Connector * c;
  f << "\n<g inkscape:label='Connectors' inkscape:groupmode='layer' id='layer_connectors'\n"
    <<  "style='opacity:0.6;' >\n";
  for(int i = 0; i < (int)clad->connectors.size(); ++i) {

    c = clad->connectors[i];
    int posX1 = datePX(c->fromWhen, clad) + xPX;
    int posX2 = datePX(c->toWhen, clad) + xPX;
    int posY1 = c->offsetA * oPX + topOffset;
    int posY2 = c->offsetB * oPX + topOffset;
    int sign;
    if(c->from->offset < c->to->offset) sign = 1;
    else sign = -1;
    string connectorDot = "";
    if(clad->connectorDots == 1) connectorDot = " marker-start='url(#__connector_" + int2str(i) + ")'";
    string dash = "";
    if(clad->connectorsDashed == 1) dash = "stroke-dasharray='" + int2str(c->thickness) + "," + int2str(c->thickness) + "'";
    f << "  <line x1='" << posX1 << "' y1='" << posY1 + sign * lPX/2 << "' x2='" << posX2 << "' y2='" << posY2
      << "' stroke='#" << c->color.hex << "' stroke-width='" << c->thickness << "' " << dash << connectorDot << " />\n";
  }
  f << "</g>\n";


  // Node Lines
  f << "\n<g inkscape:label='Lines' inkscape:groupmode='layer' id='layer_lines'\n"
    << " style='fill:none;stroke-width:" << lPX << ";' >\n";
  if(2 <= clad->derivType && clad->derivType <= 5) clad->nodesPreorder();

  for(int i = 0; i < (int)clad->nodes.size(); ++i) {

    n = clad->nodes[i];
    int sign;
    int startX = datePX(n->start, clad) + xPX;
    int stopX = datePX(n->stop, clad) + xPX;
    int posY = n->offset * oPX + topOffset;
    f << "  <path id='__line_"<< validxml(n->name, true) <<"' d='M ";
    if(n->parent != NULL) {

      int dType = clad->derivType;

      if(n->offset < n->parent->offset) sign = 1;
      else sign = -1;
      int posYparent = n->parent->offset * oPX + topOffset;
      if(dType < 1 || 5 < dType)
        posYparent -= sign * int((lPX * (1 + (sqrt(n->parent->size)-1) * clad->bigParent))/2);

      if(dType == 0)
        f << startX << " " << posYparent << " L ";
      else if(dType == 1)
        f << datePX(n->parent->start, clad) + xPX << " " << posYparent << " L ";
      else if(dType == 2) {
        int startPar = datePX(n->parent->start, clad) + xPX;
        int xdiff = sign*(posYparent-posY);
        if(startX - xdiff < startPar) xdiff = startX - startPar;
        f << startX - xdiff << " " << posYparent << " L ";
      }
      else if(3 <= dType && dType <= 5) {

        int startPar = datePX(n->parent->start, clad) + xPX;
        int xdiff = yrPX;   // one year scaling
        if(dType == 4) {    // quadratic root scaling
          xdiff *= (n->offset - n->parent->offset);
          if(xdiff < 0) xdiff = -xdiff;
          //~ xdiff = log(xdiff) *20;
          xdiff = int(sqrt(xdiff) *5);
        }
        if(startX - xdiff < startPar || dType == 5) xdiff = startX - startPar;
        int startpoint = startX - xdiff;
        f << startpoint << " " << posYparent << " C "
          << startX - 0.25*xdiff << "," << posYparent << " "
          << startX - 0.75*xdiff << "," << posY << " "
          << startX << "," << posY
          << " L ";

      }


    }
    f << startX << " " << posY << " L " << stopX << " " << posY
      << "' stroke='#"<< n->color.hex << "'";
//~ f << " style='stroke-width:" << lPX * (1 + (sqrt(n->size-1)) * clad->bigParent) << ";'";  // is more "exact"
    f << " style='stroke-width:" << lPX * (1 + (sqrt(n->size)-1) * clad->bigParent) << ";'";  // looks better
    if(n->stop < clad->endOfTime && clad->stopFadeOutPX != 0)
      f << " marker-end='url(#__stop_" << validxml(n->name, true) << ")'";
    f << " />\n";

  }
  f << "</g>\n";


  // Dots
  string style;
  if     (clad->dotType == 0) style = "stroke:none;";
  else if(clad->dotType == 1) style = "stroke-width:" + int2str(clad->lineWidth / 2) + ";fill:#" + clad->mainBackground.hex + ";";
  f << "\n<g inkscape:label='Dots' inkscape:groupmode='layer' id='layer_dots'\n"
    << " style='" << style << "' >\n";
  for(int i = 0; i < (int)clad->nodes.size(); ++i) {

    n = clad->nodes[i];
    int posX = datePX(n->start, clad) + xPX;
    int posY = n->offset * oPX + topOffset;
    string dotprops;
    if     (clad->dotType == 0) dotprops = "fill='#" + n->color.hex + "' stroke='none'";
    else if(clad->dotType == 1) dotprops = "stroke='#" + n->color.hex + "'";

    f << "  <circle id='__dot_" << validxml(n->name, true) << "' cx='" << posX << "' cy='" << posY
      << "' r='" << clad->dotRadius * (1 + (sqrt(sqrt(n->size))-1)*clad->bigParent) << "' " << dotprops << " />\n";

    for(int j = 0; j < (int)n->nameChanges.size(); ++j) {
      posX = datePX(n->nameChanges[j].date, clad) + xPX;
      f << "    <circle cx='" << posX << "' cy='" << posY << "' r='" << clad->smallDotRadius * (1 + (sqrt(sqrt(n->size))-1)*clad->bigParent) << "' " << dotprops << " />\n";
    }

  }
  f << "</g>\n";


  // Icons
  f << "\n<g inkscape:label='Icons' inkscape:groupmode='layer' id='layer_icons'>\n";
  string iconfile, format;
  for(int i = 0; i < (int)clad->nodes.size(); ++i) {

    n = clad->nodes[i];
    iconfile = n->iconfile;
    format = getExt(iconfile);
    if(format == "") continue;

    string rotate;
    string icon;
    int iconWidth = 0;
    int iconHeight = 0;

    if(format == "svg") icon = SVG_body(iconfile, iconWidth, iconHeight);
    else if(format == "png") icon = base64_png(iconfile, iconWidth, iconHeight);

    int posX = datePX(n->start, clad) + xPX - iconWidth/2;
    int posY = n->offset * oPX + topOffset - iconHeight/2;

    if(clad->orientation == oTB)
      rotate = "matrix(0,1,1,0,0,1) translate(" + int2str( -posX + posY +(-iconWidth+iconHeight)/2 ) + "," + int2str( posX - posY + (iconWidth-iconHeight)/2 ) + ")";
    if(clad->orientation == oRL)
      rotate = "matrix(-1,0,0,1,0,1) translate(-" + int2str(2 * posX + iconWidth) + ",0)";
    if(clad->orientation == oBT)
      rotate = "rotate(90," + int2str(posX + iconWidth/2) + "," + int2str(posY + iconWidth/2) + ")";

    if(format == "svg") {

      f << "  <g transform='" << rotate << " translate(" << posX << "," << posY <<")' >\n"
        << icon
        << "  </g>\n";

    } else if(format == "png") {
      f << "  <image id='__icon_" << validxml(n->name, true) << "'" << " transform='" << rotate << "'"
        << " x='" << posX << "' y='" << posY << "' width='" << iconWidth << "' height='" << iconHeight << "'\n"
        << "    xlink:href='data:image/" << format << ";base64," << icon << "' />\n";

    } else throw "unknown icon file format: " + format + "\n       accepted formats: svg, png";

  }
  f << "</g>\n";


  // Labels
  f << "\n<g inkscape:label='Labels' inkscape:groupmode='layer' id='layer_labels' " << retransformLabels << "\n"
    << " style='font-size:" << clad->labelFontSize << "px;stroke:none;fill:#" << clad->labelFontColor.hex << ";font-family:"
    << clad->labelFont << ";-inkscape-font-specification:" << clad->labelFont << ";' >\n";
  for(int i = 0; i < (int)clad->nodes.size(); ++i) {

    n = clad->nodes[i];
    string href = "", hrefend = "";

    if(clad->descriptionType == 1) {
      href = "<a xlink:href='" + validxml(n->description, false) + "'>";
      hrefend = "</a>";
    }

    int posX = datePX(n->start, clad) + xPX + clad->dotRadius;
    int posY = n->offset * oPX + topOffset - dirty_hack_ex/2 - int(lPX*((sqrt(n->size)-1) * clad->bigParent)/2);
    int posXwName = posX + strlenpx(n->name, clad) + dirty_hack_em;  // + dirty_hack_em is experimental
    int alignmentBGx = posX - dirty_hack_em/4;
    string alignment = "";

    if(clad->orientation == oTB) {

      posX = n->offset * oPX + topOffset;
      posY = datePX(n->start, clad) + xPX - clad->dotRadius - dirty_hack_ex/5;

      alignment = "style='text-anchor:middle;'";
      alignmentBGx = posX - strlenpx(n->name, clad) / 2;

    } else if(clad->orientation == oRL) {

      posX = width - posX - strlenpx(n->name, clad);
      alignmentBGx = width - alignmentBGx - strlenpx(n->name, clad);

    } else if(clad->orientation == oBT) {

      posX = n->offset * oPX + topOffset;
      posY = canvasHeight - (datePX(n->start, clad) + xPX - clad->dotRadius - dirty_hack_ex * 7/5);

      alignment = "style='text-anchor:middle;'";
      alignmentBGx = posX - strlenpx(n->name, clad) / 2;

    }

    if(clad->labelBGOpacity > 0)
      f << "  <rect x='" << alignmentBGx << "' y='" << posY - dirty_hack_ex *6/5 << "' width='" << strlenpx(n->name, clad)
        << "' height='" << dirty_hack_ex *7/5 << "' fill='#" << clad->mainBackground.hex << "' opacity='" << double(clad->labelBGOpacity)/100 
//~ << "' height='" << dirty_hack_ex *7/5 << "' fill='#a00' opacity='" << double(clad->labelBGOpacity)/100 
        << "'  rx='5' ry='5' />\n";

    f << "  " << href << "<text x='"<< posX <<"' y='"<< posY <<"' " << alignment << " >" << validxml(n->name, false) <<"</text>" << hrefend << "\n";


    string alignmentNameChange = "";
    for(int j = 0; j < (int)n->nameChanges.size(); ++j) {

      if(clad->nameChangeType == nc_outside) {  // nameChange outside the dot

        posX = datePX(n->nameChanges[j].date, clad) + xPX + clad->smallDotRadius;
        if(posX < posXwName) posX = posXwName;
        if(j != 0) posXwName = posX + strlenpx(n->nameChanges[j-1].newName, clad) + dirty_hack_em ;  // + dirty_hack_em is experimental

      } else if(clad->nameChangeType == nc_inside) {  // nameChange centered on the dot

        posX = datePX(n->nameChanges[j].date, clad) + xPX;
        posY = n->offset * oPX + topOffset + dirty_hack_ex/2;
        alignmentNameChange = "style='text-anchor:middle;'";

      }

      if(clad->orientation == oTB) {

        posX = n->offset * oPX + topOffset + clad->smallDotRadius;
        posY = datePX(n->nameChanges[j].date, clad) + xPX - clad->smallDotRadius;
        if(clad->nameChangeType == nc_inside) {
          posX -= clad->smallDotRadius;
          posY += clad->smallDotRadius + dirty_hack_ex/2;
        }

      } else if(clad->orientation == oRL) {

        posX = width - posX - strlenpx(n->nameChanges[j].newName, clad);

      } else if(clad->orientation == oBT) {

        posX = n->offset * oPX + topOffset + clad->smallDotRadius;
        posY = canvasHeight - (datePX(n->nameChanges[j].date, clad) + xPX + clad->smallDotRadius/2);
        if(clad->nameChangeType == nc_inside) {
          posX -= clad->smallDotRadius;
          posY += clad->smallDotRadius/2 + dirty_hack_ex/2;
        }

      }

      if(clad->descriptionType == 1)
        href = "<a xlink:href='" + validxml(n->nameChanges[j].description, false) + "'>";

      if(clad->labelBGOpacity > 0 && clad->nameChangeType != nc_inside)
        f << "    <rect x='" << posX - dirty_hack_em/4 << "' y='" << posY - dirty_hack_ex *6/5 << "' width='" << strlenpx(n->nameChanges[j].newName, clad)
          << "' height='" << dirty_hack_ex *7/5 << "' fill='#" << clad->mainBackground.hex << "' opacity='" << double(clad->labelBGOpacity)/100 << "'  rx='5' ry='5' />\n";
//~ << "' height='" << dirty_hack_ex *7/5 << "' fill='#a00' opacity='" << double(clad->labelBGOpacity)/100 << "'  rx='5' ry='5' />\n";

      f << "    " << href << "<text x='"<< posX <<"' y='"<< posY <<"' " << alignmentNameChange << ">" << validxml(n->nameChanges[j].newName, false) <<"</text>" << hrefend << "\n";

    }

  }
  f << "</g>\n";


  // Year Lines with the year numbers
  f << "\n<g inkscape:label='Yearlines' inkscape:groupmode='layer' id='layer_yearlines' " << retransformYearlines << "\n"
    << " style='stroke:none;fill:url(#__yearline);' >\n";
  yrlinePX -= 3 * oPX / 2;  // remove small margin
  dirty_hack_ex = int(clad->yearLineFontSize / 1.375 * clad->fontCorrectionFactor);  // CSS ex unit
  if(yrlinePX > 0) {

  xPX -= clad->prependYears *yrPX;  // add prepended years

    f << "  <rect x='" << xPX - 10 << "' y='" << topOffset - yrlinePX - 3*oPX/2 << "' rx='5' ry='5' width='" << width - xPX + 10<< "' height='" << yrlinePX << "' />\n"
      << "  <rect x='" << xPX - 10 << "' y='" << height - yrlinePX << "' rx='5' ry='5' width='" << width - xPX + 10 << "' height='" << yrlinePX << "' />\n"
      << "  <g style='font-size:" << clad->yearLineFontSize << "px;stroke:none;fill:#" << clad->yearLineFontColor.hex << ";font-family:" << clad->yearLineFont << ";-inkscape-font-specification:" << clad->yearLineFont << ";text-anchor:middle;' >\n";
    for(int i = 0; i <= clad->endOfTime.year - clad->beginningOfTime.year + clad->prependYears + clad->appendYears; i += clad->yearLabelInterval) {
      int posX = yrPX * i + yrPX / 2 + xPX;
      int posY = topOffset - 3*oPX/2 - yrlinePX/2 + dirty_hack_ex / 2;
      int yeartext = clad->beginningOfTime.year + i - clad->prependYears;
      if(clad->orientation == oRL) yeartext = clad->endOfTime.year - i;
      f << "    <text x='" << posX << "' y='" << posY << "'><tspan>" << yeartext << "</tspan></text>\n"
        << "    <text x='" << posX << "' y='" << height - yrlinePX/2 + dirty_hack_ex/2 << "'><tspan>" << yeartext << "</tspan></text>\n";
    }
    f << "  </g>\n";

  xPX += clad->prependYears *yrPX;  // remove prepended years
  }
  f << "</g>\n";


  // Orientation STOP
  if(clad->orientation != oLR)
    f << "</g><!-- END orientation transform -->\n";


  // The infobox
  dirty_hack_ex = int(clad->infoBoxTitleSize / 1.375 * clad->fontCorrectionFactor);
  int dirty_hack_ex2 = int(clad->infoBoxTextSize / 1.375 * clad->fontCorrectionFactor * 1.8);
  int posX = clad->infoBoxX + dirty_hack_ex * 1/2;
  int posY = clad->infoBoxY + dirty_hack_ex *5/3;
  f << "\n<g inkscape:label='Infobox' inkscape:groupmode='layer' id='layer_infobox'\n"
    << " style='stroke:none;fill:#" << clad->infoBoxFontColor.hex << ";font-family:" << clad->infoBoxFont << ";-inkscape-font-specification:" << clad->infoBoxFont << ";' >\n"
    << "  <rect x='" << clad->infoBoxX << "' y='" << clad->infoBoxY << "' width='" << clad->infoBoxWidth << "' height='" << clad->infoBoxHeight << "' rx='10' ry='10' fill='#000' filter='url(#__infobox_shadow)' opacity='0.6' />\n"
    << "  <rect x='" << clad->infoBoxX << "' y='" << clad->infoBoxY << "' width='" << clad->infoBoxWidth << "' height='" << clad->infoBoxHeight << "' rx='10' ry='10' fill='url(#__infobox_fill)' stroke='url(#__infobox_stroke)' stroke-width='2' />\n";
  if(clad->infoBoxTitleSize > 0)
    f << "  <text x='" << posX << "' y='" << posY << "'><tspan style='font-size:" << clad->infoBoxTitleSize << "px;font-weight:bold;'>" << clad->infoBoxTitle << "</tspan></text>\n";
  if(clad->infoBoxTextSize > 0 )
    for(int i = 0; i < (int)clad->infoBoxText.size(); ++i)
      f << "  <text x='" << posX << "' y='" << posY + dirty_hack_ex + dirty_hack_ex2 * i << "'><tspan style='font-size:" << clad->infoBoxTextSize << "px;'>" << clad->infoBoxText[i] << "</tspan></text>\n";
  f << "</g>\n";

  f << "\n<!-- BEGIN additional images -->\n";


  // Additional PNG images
  Image * image;
  f << "\n<g inkscape:label='Included PNG Images' inkscape:groupmode='layer' id='layer_included_png'>\n";
  for(int i = 0; i < (int)clad->includePNG.size(); ++i) {
    image = clad->includePNG[i];

    int imgWidth = 0;
    int imgHeight = 0;
    string data = base64_png(image->filename, imgWidth, imgHeight);

    f << "  <image id='__png_" << int2str(i) << "' x='" << image->x + xPX - clad->prependYears*yrPX << "' y='" << image->y + topOffset << "' width='" << imgWidth << "' height='" << imgHeight << "'\n"
      << "    xlink:href='data:image/png;base64," << data << "' />\n";
  }
  f << "</g>\n";


  // Additional SVG images
  f << "\n<g inkscape:label='Included SVG Images' inkscape:groupmode='layer' id='layer_included_svg'>\n";
  for(int i = 0; i < (int)clad->includeSVG.size(); ++i) {

    int void1, void2;
    image = clad->includeSVG[i];
    string data = SVG_body(image->filename, void1, void2);
    f << "  <g transform='translate(" << image->x + xPX - clad->prependYears*yrPX << "," << image->y + topOffset << ")' >\n"
      << data
      << "  </g>\n";

  }
  f << "</g>\n";

  f << "\n<!-- END additional images -->\n";


  f << "\n</svg>\n";

}


// Some lame heuristics, only for variable width ASCII chars
int GeneratorSVG::strlenpx(std::string str, Cladogram * clad) {

  if(clad->asciiStrings == 0) return str.size() * dirty_hack_em;

  double len = 0;
  char c;
  for(int i = 0; i < (int)str.size(); ++i) {

    static const double digit[] =
       { 1.05, 0.9, 1.05, 1.05, 1.05, 1.05, 1.05, 1.05, 1.05, 1.05};  // 0 - 9

    static const double alpha[] = { 1.05,  // a
           1.05, 0.9, 1.05, 1.05, 0.475,   // b c d e f
           1.05, 1.05, 0.46, 0.46, 0.9,    // g h i j k
           0.46, 1.475, 1.05, 1.05, 1.05,  // l m n o p
           1.05, 0.65, 0.9, 0.475, 1.05,   // q r s t u
           0.9, 1.35, 0.9, 0.9, 0.9 };     // v w x y z

    static const double ALPHA[] = { 1.225,  // A
       1.225, 1.325, 1.325, 1.225, 1.05,    // B C D E F
       1.325, 1.325, 0.475, 0.9, 1.225,     // G H I J K
       1.05, 1.475, 1.325, 1.325, 1.225,    // L M N O P
       1.325, 1.325, 1.225, 1.05, 1.325,    // Q R S T U
       1.225, 1.625, 1.225, 1.225, 1.05 };  // V W X Y Z

    static const double misc1[] = { 0.6,  // space
        0.475, 0.65, 1.05, 1.05, 1.65,    // ! " # $ %
        1.05, 0.5, 0.65, 0.65, 0.75,      // & ' ( ) *
        1.05, 0.475, 0.65, 0.475, 0.9 };  // + , - . /

    static const double misc2[] =
       { 0.5, 0.5, 1, 1.05, 1, 1.05, 1.75 };  // : ; < = > ? @

    static const double misc3[] =
       { 0.5, 0.5, 1, 0.9, 1.05, 0.65 };  // [ \ ] ^ _ `

    static const double misc4[] =
       { 0.625, 0.5, 0.625, 1.05 };  // { | } ~

    c = str[i];
    if     ('0' <= c && c <= '9') len += digit[int(c - '0')];
    else if('a' <= c && c <= 'z') len += alpha[int(c - 'a')];
    else if('A' <= c && c <= 'Z') len += ALPHA[int(c - 'A')];
    else if(' ' <= c && c <= '/') len += misc1[int(c - ' ')];
    else if(':' <= c && c <= '@') len += misc2[int(c - ':')];
    else if('[' <= c && c <= '`') len += misc3[int(c - '[')];
    else if('{' <= c && c <= '~') len += misc4[int(c - '{')];
    else len += 1.0;  // everything else

  }
  return int(len * double(dirty_hack_em));
}

// Escape reserved XML entities. Whitespace breaks refs, but is OK in content.
string validxml(string str, bool ws) {
  unsigned char c;
  string rep;
  for(int i = 0; i < (int)str.size(); ++i) {
    c = (unsigned char)str[i];
    if     (c == '&')  rep = "&amp;";
    else if(c == '<')  rep = "&lt;";
    else if(c == '>')  rep = "&gt;";
    else if(c == '"')  rep = "\"";
    else if(c == '\'') rep = "'";
    else if(c == ' ' && ws == true) rep = "__";
    else continue;
    str.replace(i, 1, rep);
    i += rep.size() - 1;
  }
  return str;
}

/*
string validxml(string str) {
  // We could simply encode the whole string in base64, but then the output
  // file would be much less human readable, which is bad.
  for(int i = 0; i < (int)str.size(); ++i) {
    const unsigned char c = (unsigned char)str[i];

    if(  c >= 127 || c >= 123 ||              // stuff is redundant here, but
       ((c <= 32 || c <= 47) && c != '-') ||  // leave it to highlight the ideas
       ( c >= 58 && c <= 64 ) ||
       ( c >= 91 && c <= 96 ) ) {

      string rep = "_" + int2str((int)c);
      if(rep.size() == 2) rep.insert(1, "0");
      if(rep.size() == 3) rep.insert(1, "0");  // push to 4
      if(c == ' ') rep = "__";
      str.replace(i, 1, rep);
      i += rep.size()-1;

    }
  }
  return str;
}
*/

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

    const int size = fp.tellg();
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
