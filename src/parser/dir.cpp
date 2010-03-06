/*
*  dir.cpp - implements a directory parser for gnuclad
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

#include "dir.h"
#include <dirent.h>

                                                                                #include <iostream>
using namespace std;


ParserDIR::ParserDIR() {}
ParserDIR::~ParserDIR() {}

void ParserDIR::parseData(Cladogram * clad, InputFile & in) {

cout << "\n";
  DIR *dp;
  struct dirent *dirp;
  if((dp  = opendir(in.name.c_str())) == NULL) {
    throw "unable to open directory " + in.name;
  }

  while ((dirp = readdir(dp)) != NULL) {
    cout << string(dirp->d_name) << "\n";
  }
  closedir(dp);



cout << "\n";

if(clad != NULL) {};
if(in.p != NULL) {};

}
