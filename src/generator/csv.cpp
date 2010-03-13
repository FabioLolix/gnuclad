/*
*  csv.cpp - implements a CSV generator for gnuclad
*
*  Copyright (C) 2010 Donjan Rodic <donjan@dyx.ch>
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

#include "csv.h"

using namespace std;


GeneratorCSV::GeneratorCSV() {}
GeneratorCSV::~GeneratorCSV() {}

void GeneratorCSV::writeData(Cladogram * clad, OutputFile & out) {

  ofstream & f = *(out.p);

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
    n = clad->nodes[i];
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


  f << "\"//\",\"Open/save this file as a standard CSV  <=>  comma (,) separated and double quotes (\"\") as text delimiter.\"" << tail2 << "\n"
    << "\"//\",\"You can safely insert commas or double quotes into data fields.\"" << tail2 << "\n"
    << "\"//\",\"EXCEPTION: Don't insert a comma right after a double quote, since this terminates the field.\"" << tail2 << "\n"
    << "\"//\",\"When editing with OO.org Calc, use the single quote (') for three-number-dates to circumvent automatic conversion.\"" << tail2 << "\n"
    << "\"//\"" << tailWidth << "\n"
    << "\"//!\",\"gnuclad parses this correctly no matter in what order the nodes are. You can swap any lines.\"" << tail2 << "\n"
    << "\"//!\"" << tailWidth << "\n"
    << "\"//!\",\"Empty lines get ignored, the first column controls the rest of the line:\"" << tail2 << "\n"
    << "\"//!\",\"# = comment, N = node, C = connector, D = domain\"" << tail2 << "\n"
    << "\"//!\",\"SVG = SVG image to include, PNG = PNG image to include\"" << tail2 << "\n"
    << "\"//!\"" << tailWidth << "\n"
    << tailWidth << "\n";


  // Nodes
  f << "\"#\",\"Nodes\"" << tail2 << "\n"
    << "\"#\",\"Name\",\"Color\",\"Parent\",\"Start\",\"Stop\",\"Icon\",\"Description\",\"[Namechange\",\"When\",\"Description\",\"[Namechange\",\"When\",\"Description\",\". . . ]]\"" << tail15 << "\n";
  for(int i = 0; i < (int)clad->nodes.size(); ++i) {

    n = clad->nodes[i];

    tailN = "";
    for(int j = 0; j < width-fixedFieldsNode-(int)n->nameChanges.size()*3; ++j)
      tailN += ",";

    string stopdate = Date2str(n->stop);       // cosmetic hack:
    if( !(n->stop < clad->endOfTime) )         // if node didn't stop yet,
      stopdate = "";                           // set empty stop date

    f << "\"N\",\"" << n->name << "\",\"#" << n->color.hex << "\",\"" 
      << n->parentName << "\",\""
      << Date2str(n->start) << "\",\"" << stopdate << "\",\""
      << n->iconfile << "\",\"" << n->description << "\"";

    for(int j = 0; j < (int)n->nameChanges.size(); ++j) {
      NameChange * nc = &(n->nameChanges[j]);
      f << ",\"" << nc->newName << "\",\"" << Date2str(nc->date) << "\",\""
        << nc->description << "\"";
    }

    f << tailN << "\n";
  }
  
  f << tailWidth << "\n";


  // Connectors
  f << "\"#\",\"Connectors\"" << tail2 << "\n"
    << "\"#\",\"Leaving 'To When' empty will result in using 'From When' as this value\"" << tail2 << "\n"
    << "\"#\",\"From When\",\"From\",\"To When\",\"To\",\"Thickness\",\"Color\"" << tailC << "\n";
  for(int i = 0; i < (int)clad->connectors.size(); ++i) {

    c = clad->connectors[i];

    string fromWhen = Date2str(c->fromWhen);
    string toWhen = Date2str(c->toWhen);                // cosmetic hack:
    if( !(fromWhen < toWhen) && !(toWhen < fromWhen) )  // if they are equal
      toWhen = "";                                      // set empty toWhen date

    f << "\"C\",\"" << fromWhen << "\",\"" << c->fromName << "\",\""
      << toWhen << "\",\"" << c->toName << "\",\""
      << int2str(c->thickness) << "\",\"#" << c->color.hex << "\""
      << tailC << "\n";
    
  }

  f << tailWidth << "\n";


  // Domains
  f << "\"#\",\"Domains\"" << tail2 << "\n"
    << "\"#\",\"Node\",\"Color\",\"Intensity\"" << tailD << "\n";
  for(int i = 0; i < (int)clad->domains.size(); ++i) {

    d = clad->domains[i];

    f << "\"D\",\"" << d->nodeName << "\",\"#" << d->color.hex
      << "\",\"" << int2str(d->intensity) << "\"" << tailD << "\n";
  }

  f << tailWidth << "\n";


  // Images
  f << "\"#\",\"Images\"" << tail2 << "\n"
    << "\"#\",\"File Path\",\"x Position\",\"y Position\"" << tailIm << "\n";
  for(int i = 0; i < (int)clad->includeSVG.size(); ++i) {

    im = clad->includeSVG[i];

    f << "\"SVG\",\"" << im->filename << "\",\"" << int2str(im->x)
      << "\",\"" << int2str(im->y) << "\"" << tailIm << "\n";
  }
  for(int i = 0; i < (int)clad->includePNG.size(); ++i) {

    im = clad->includePNG[i];

    f << "\"PNG\",\"" << im->filename << "\",\"" << int2str(im->x)
      << "\",\"" << int2str(im->y) << "\"" << tailIm << "\n";
  }
}
