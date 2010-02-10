/*
*  generatorSVG.h - SVG generator header for gnuclad
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


#ifndef GENERATORSVG_H_
#define GENERATORSVG_H_

#include "gnuclad.h"


class GeneratorSVG: public Generator {
  public:

  GeneratorSVG();
  void writeData(Cladogram * clad, std::ofstream * fp);
};

std::string validxml(std::string str);
std::string SVG_defs(std::string filename);
std::string SVG_body(std::string filename, int &width, int &height);
std::string base64_png(std::string filename, int &width, int &height);

#endif
