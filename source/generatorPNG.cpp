/*
*  generatorPNG.cpp - implements a PNG generator for gnuclad
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

#include "generatorPNG.h"

#include <iostream>                     //<<<<<<<<<<<<<<<<<<<<<<<< REMOVE later

using namespace std;

// open new tmp file, generate svg into it,
// use inkscape TMP.svg -D --export-png=TMP.png,
// open tmp.png file and pool it into our target file

// or use libpng and it's compression zlib?
// or depend on librsvg2-bin (rsvg-convert) instead of inkscape? might only work if it doesn't take snaps.


GeneratorPNG::GeneratorPNG() {
}

void GeneratorPNG::writeData(Cladogram * clad, ofstream * fp) {

  cout << "\nTrying to generate PNG...\n"
       << "Trying really hard...\n"
       << "Can't... do... it :(\n"
       << "\nSorry, I really tried. Maybe you could generate a SVG and convert"
       << " it with a tool like Inkscape?";

  throw "PNG generator not implemented yet!";

  if(clad != NULL || fp != NULL) {};
}
