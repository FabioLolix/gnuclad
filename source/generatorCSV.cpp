/*
*  generatorCSV.cpp - implements a CSV generator for gnuclad
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

#include "generatorCSV.h"

#include <iostream>                             //<<<<<<<<<<<<<<<<<<<<<<<< REMOVE later

using namespace std;


GeneratorCSV::GeneratorCSV() {
}

void GeneratorCSV::writeData(Cladogram * clad, ofstream * fp) {

  Node * n;
  Connector * c;
  Domain * d;
  Image * im;
  int fixedFieldsNode = 8;
  int fixedFieldsConnector = 7;
  int fixedFieldsDomain = 4;
  int fixedFieldsImage = 4;

  int width = fixedFieldsNode;
  for(int i = 0; i < (int)clad->nodes.size(); ++i) {
    n = clad->nodes.at(i);
    if(width < fixedFieldsNode + (int)n->nameChanges.size()*3)
      width = fixedFieldsNode + n->nameChanges.size()*3;
  }

  string tailWidth = "";
  string tail2 = "";
  string tail15 = "";
  string tailC = "";
  string tailD = "";
  string tailIm = "";
  string tailN;
  for(int i = 0; i < width - 1; ++i)
    tailWidth += ",";
  for(int i = 0; i < width - 2; ++i)
    tail2 += ",";
  for(int i = 0; i < width - 15; ++i)
    tail15 += ",";
  for(int i = 0; i < width - fixedFieldsConnector; ++i)
    tailC += ",";
  for(int i = 0; i < width - fixedFieldsDomain; ++i)
    tailD += ",";
  for(int i = 0; i < width - fixedFieldsImage; ++i)
    tailIm += ",";


  *fp << "\"//\",\"Open/save this file as a standard CSV  <=>  comma (,) separated and double quotes (\"\") as text delimiter.\"" << tail2 << "\n"
      << "\"//\",\"You can safely insert commas or double quotes into data fields.\"" << tail2 << "\n"
      << "\"//\",\"EXCEPTION: Don't insert a comma right after a double quote, since this terminates the field.\"" << tail2 << "\n"
      << "\"//\",\"When editing with OO.org Calc, use the single quote (') for three-number-dates to circumvent automatic conversion.\"" << tail2 << "\n"
      << "\"//\"" << tailWidth << "\n"
      << "\"//!\",\"gnuclad parses this correctly no matter in what order the nodes are. You can swap any lines.\"" << tail2 << "\n"
      << "\"//!\"" << tailWidth << "\n"
      << "\"//!\",\"Empty lines get ignored, the first column controls the rest of the line:\"" << tail2 << "\n"
      << "\"//!\",\"# = comment, N = node, C = connector, D = domain, SVG = SVG image to include\"" << tail2 << "\n"
      << "\"//!\"" << tailWidth << "\n"
      << tailWidth << "\n";


  // Nodes
  *fp << "\"#\",\"Nodes\"" << tail2 << "\n"
      << "\"#\",\"Name\",\"Color\",\"Parent\",\"Start\",\"Stop\",\"Icon\",\"Description\",\"[Namechange\",\"When\",\"Description\",\"[Namechange\",\"When\",\"Description\",\". . . ]]\"" << tail15 << "\n";
  for(int i = 0; i < (int)clad->nodes.size(); ++i) {

    n = clad->nodes.at(i);

    tailN = "";
    for(int i = 0; i < width-fixedFieldsNode-(int)n->nameChanges.size()*3; ++i)
      tailN += ",";

    string stopdate = Date2str(n->stop);       // cosmetic hack:
    if( !(n->stop < clad->endOfTime) )         // if node didn't stop yet,
      stopdate = "";                           // set empty stop date

    *fp << "\"N\",\"" << n->name << "\",\"#" << n->color.hex << "\",\"" 
        << n->parentName << "\",\""
        << Date2str(n->start) << "\",\"" << stopdate << "\",\""
        << n->icon.filename << "\",\"" << n->description << "\"";

    for(int j = 0; j < (int)n->nameChanges.size(); ++j) {
      NameChange * nc = &(n->nameChanges.at(j));
      *fp << ",\"" << nc->newName << "\",\"" << Date2str(nc->date) << "\",\""
          << nc->description << "\"";
    }

    *fp << tailN << "\n";
  }
  
  *fp << tailWidth << "\n";


  // Connectors
  *fp << "\"#\",\"Connectors\"" << tail2 << "\n"
      << "\"#\",\"Leaving 'To When' empty will result in using 'From When' as this value\"" << tail2 << "\n"
      << "\"#\",\"From When\",\"From\",\"To When\",\"To\",\"Thickness\",\"Color\"" << tailC << "\n";
  for(int i = 0; i < (int)clad->connectors.size(); ++i) {

    c = clad->connectors.at(i);

    string fromWhen = Date2str(c->fromWhen);
    string toWhen = Date2str(c->toWhen);                // cosmetic hack:
    if( !(fromWhen < toWhen) && !(toWhen < fromWhen) )  // if they are equal
      toWhen = "";                                      // set empty toWhen date

    *fp << "\"C\",\"" << fromWhen << "\",\"" << c->fromName << "\",\""
        << toWhen << "\",\"" << c->toName << "\",\""
        << int2str(c->thickness) << "\",\"#" << c->color.hex << "\""
        << tailC << "\n";
    
  }

  *fp << tailWidth << "\n";


  // Domains
  *fp << "\"#\",\"Domains\"" << tail2 << "\n"
      << "\"#\",\"Distribution\",\"Color\",\"Intensity\"" << tailD << "\n";
  for(int i = 0; i < (int)clad->domains.size(); ++i) {

    d = clad->domains.at(i);

    *fp << "\"D\",\"" << d->nodeName << "\",\"#" << d->color.hex
        << "\",\"" << int2str(d->intensity) << "\"" << tailD << "\n";
  }

  *fp << tailWidth << "\n";


  // Images
  *fp << "\"#\",\"Images\"" << tail2 << "\n"
      << "\"#\",\"File Path\",\"x Position\",\"y Position\"" << tailIm << "\n";
  for(int i = 0; i < (int)clad->includeSVG.size(); ++i) {

    im = clad->includeSVG.at(i);

    *fp << "\"SVG\",\"" << im->filename << "\",\"" << int2str(im->x)
        << "\",\"" << int2str(im->y) << "\"" << tailIm << "\n";
  }

}
