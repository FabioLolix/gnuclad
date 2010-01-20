/*
*  parserCSV.cpp - implements an CSV parser for gnuclad
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

#include "parserCSV.h"

using namespace std;


ParserCSV::ParserCSV() {
}

void ParserCSV::parseData(Cladogram * clad, ifstream * fp) {

  string line;
  int count = 1;
  int fixedFields = 8;

  while( !fp->eof() && fp->good() ) {

    getline(*fp, line);
    ++count;

    vector<string> entry;
    //~ explode(line, '\t', &entry);
    explodeSafely(line, ',', '"', &entry);

    // remove double quotes
    for(int i = 0; i < (int)entry.size(); ++i) {
      string * e = &entry.at(i);
      if(e->size() == 0) continue;
      if(e->at(0) == '"') *e = e->substr(1, e->size() - 1);
      if(e->at(e->size() - 1) == '"') *e = e->substr(0,e->size() - 1);
    }

    if(entry.size() == 0) continue;

    string what;
    if     (entry.at(0) == "N") what = "node";
    else if(entry.at(0) == "C") what = "connector at";
    else if(entry.at(0) == "D") what = "domain of";

    try {
      if(entry.at(0) == "N") {  // add a node

          if((int)entry.size() < fixedFields) throw 0;

          Node * node = clad->addNode(entry.at(1));
          node->color = Color(entry.at(2));
          if(entry.at(3) != "-" && entry.at(3) != "0")
            node->parentName = entry.at(3);
          node->start = Date(entry.at(4));
          node->stop = Date(entry.at(5));
          if(entry.at(6) != "-")
            node->icon = entry.at(6);
          if(entry.at(7) != "-")
            node->description = entry.at(7);

          // get the name changes
          for(int i = fixedFields; i < (int)entry.size()-1; i += 2)
            if(entry.at(i) != "")
              node->addNameChange(entry.at(i), Date(entry.at(i+1)) );

      } else if(entry.at(0) == "C") {  // add a connector

        if((int)entry.size() < 6) throw 0;

        Connector * c = clad->addConnector();
        c->when = Date(entry.at(1));
        c->fromName = entry.at(2);
        c->toName = entry.at(3);
        c->thickness = str2int( entry.at(4) );
        c->color = Color(entry.at(5));

      } else if(entry.at(0) == "D") {  // add a domain

        if((int)entry.size() < 4) throw 0;

        Domain * domain = clad->addDomain(entry.at(1));
        domain->color = Color(entry.at(2));
        domain->intensity = str2int( entry.at(3) );

      }
    } catch (...) {
      throw "invalid entry at line " + int2str(count - 1)
            + " (" + what + " " + entry.at(1) + ")";
    }

  }

}
