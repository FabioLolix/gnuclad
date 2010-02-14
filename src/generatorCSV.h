/*
*  generatorCSV.h - CSV generator header for gnuclad
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


#ifndef GENERATORCSV_H_
#define GENERATORCSV_H_

#include "gnuclad.h"


class GeneratorCSV: public Generator {
  public:

  GeneratorCSV();
  ~GeneratorCSV();
  void writeData(Cladogram * clad, std::ofstream * fp);
};


#endif
