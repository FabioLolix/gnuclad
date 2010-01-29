/*
*  parserGCT.cpp - implements a GCT parser for gnuclad
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

#include "parserGCT.h"
#include <iostream>

using namespace std;


ParserGCT::ParserGCT() {
}

void ParserGCT::parseData(Cladogram * clad, ifstream * fp) {

  string line;
  int count = 1;
  int reqFields = 7;

  while( !fp->eof() && fp->good() ) {

    getline(*fp, line);
    ++count;

    vector<string> entry;
    explode(line, ',', &entry);

    if(entry.size() == 0) continue;

    if(entry.at(0) == "N") {  // add a node
      try {

        if((int)entry.size() < reqFields) throw 0;

        Node * node = clad->addNode(entry.at(1));
        node->color = Color( entry.at(2).substr(1) );  // remove the "#"
        if(entry.at(3) != "-") node->parentName = entry.at(3);
        node->start = Date(entry.at(4));
        node->stop = Date(entry.at(5));
        if(entry.at(6) != "-") node->icon = entry.at(6);

        // get the name changes
        if( ((int)entry.size() - reqFields) % 2 != 0) throw 0;
        for(int i = reqFields; i < (int)entry.size(); i += 2)
          node->addNameChange(entry.at(i), Date(entry.at(i+1)), entry.at(i+2));

      } catch (...) {
        throw "invalid entry at line " + int2str(count)
              + " (node " + entry.at(1) + ")";
      }

    } else if(entry.at(0) == "C") {  // add a connector

      Connector * c = clad->addConnector();
      c->when = Date(entry.at(1));
      c->fromName = entry.at(2);
      c->toName = entry.at(3);
      c->thickness = str2int( entry.at(4) );
      c->color = Color( entry.at(5).substr(1) );  // remove the "#"

    } else if(entry.at(0) == "D") {  // add a domain

      Domain * domain = clad->addDomain(entry.at(1));
      domain->color = Color( entry.at(2).substr(1) );  // remove the "#"
      domain->intensity = str2int( entry.at(3) );

    }

  }

}
