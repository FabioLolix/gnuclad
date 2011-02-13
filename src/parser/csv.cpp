/*
*  csv.cpp - implements an CSV parser for gnuclad
*
*  Copyright (C) 2010-2011 Donjan Rodic <donjan@dyx.ch>
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


ParserCSV::ParserCSV() {}
ParserCSV::~ParserCSV() {}

void ParserCSV::parseData(Cladogram * clad, InputFile & in) {

  ifstream & f = *(in.p);

  string line;
  int count = 1;
  int fixedFieldsNode = 8;
  int fixedFieldsConnector = 7;
  int fixedFieldsDomain = 4;
  int fixedFieldsImage = 4;

  while( !f.eof() && f.good() ) {

    getline(f, line);
    ++count;


    vector<string> entry;
    //~ explode(line, '\t', &entry);
    explodeSafely(line, ',', '"', &entry);

    // remove double quotes
    for(int i = 0; i < (int)entry.size(); ++i) {
      string * e = &entry[i];
      if(e->size() == 0) continue;
      if(e->at(0) == '"') *e = e->substr(1, e->size() - 1);
      if(e->at(e->size() - 1) == '"') *e = e->substr(0, e->size() - 1);
    }

    if(entry.size() == 0) continue;

    string what;
    string ctl = entry[0];
    if     (ctl == "N") what = "node ";
    else if(ctl == "C") what = "connector at ";
    else if(ctl == "D") what = "domain of ";
    else if(ctl == "SVG") what = "SVG include ";
    else                what = ctl;

    try {

      if(ctl == "" || ctl[0] == '#' || ctl.substr(0,2) == "//") {}
      else if(ctl == "N") {  // add a node

        if((int)entry.size() < fixedFieldsNode) throw 0;

        Node * node = clad->addNode(entry[1]);
        node->color = Color(entry[2]);
        node->parentName = entry[3];
        node->start = Date(entry[4]);
        node->stop = Date(entry[5]);
        node->iconfile = entry[6];
        node->description = entry[7];

        // get the name changes
        for(int i = fixedFieldsNode; i < (int)entry.size()-1; i += 3)
          if(entry[i] != "" && entry[i+1] != "")
            node->addNameChange(entry[i], Date(entry[i+1]),entry[i+2]);

      } else if(ctl == "C") {  // add a connector

        if((int)entry.size() < fixedFieldsConnector) throw 0;

        Connector * c = clad->addConnector();
        c->fromWhen = Date(entry[1]);
        c->fromName = entry[2];
        if(entry[3] == "") c->toWhen = c->fromWhen;
        else c->toWhen = Date(entry[3]);
        c->toName = entry[4];
        c->thickness = str2int(entry[5]);
        c->color = Color(entry[6]);

      } else if(ctl == "D") {  // add a domain

        if((int)entry.size() < fixedFieldsDomain) throw 0;

        Domain * domain = clad->addDomain(entry[1]);
        domain->color = Color(entry[2]);
        domain->intensity = str2int(entry[3]);

      } else if(ctl == "SVG") {

        if((int)entry.size() < fixedFieldsImage) throw 0;

        Image * image = clad->addImage(entry[1], clad->includeSVG);
        image->x = str2int(entry[2]);
        image->y = str2int(entry[3]);

      } else if(ctl == "PNG") {

        if((int)entry.size() < fixedFieldsImage) throw 0;

        Image * image = clad->addImage(entry[1], clad->includePNG);
        image->x = str2int(entry[2]);
        image->y = str2int(entry[3]);

      } else throw 0;

    } catch (...) {
      throw "invalid entry at line " + int2str(count - 1)
            + " (" + what + entry[1] + ")";
    }

  }

}
