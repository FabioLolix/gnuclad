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
#include <cstring>
                                                                    #include <iostream>
using namespace std;


ParserDIR::ParserDIR() {}
ParserDIR::~ParserDIR() {}

void ParserDIR::parseData(Cladogram * clad, InputFile & in) {

cout << "\nWARNING: directory parsing is experimental!";

  clad->beginningOfTime = Date(1);
  clad->endOfTime = Date(1);
  clad->truncateFolder = true;
  clad->inVitro = true;

  clad->derivType = 1;
  clad->stopFadeOutPX = 0;
  clad->rulerMonthWidth = 0;

  string dir = in.name;

  // remove trailing folder_delimiter
  if(dir.substr(dir.size()-1) == folder_delimiter)
    dir = dir.substr(0, dir.size()-1);

  addNode(dir, Color("#00f"), "", 0, clad);
  parseDir(dir, clad, 1);

}


void ParserDIR::parseDir(std::string dirname, Cladogram * clad, int level) {

  if(clad->endOfTime.year <= level) clad->endOfTime.year = level + 1;


// SOLVE OPTIMISATION PROBLEM: opt > x2 -> make pull work

  string name;
  DIR * dir = new_indir(dirname);
  struct dirent * dirElem;

  vector<string> dirs;

  // Scan all nodes in current folder
  while ((dirElem = readdir(dir)) != NULL) {

    name = dirname + folder_delimiter + dirElem->d_name;
    if(dirElem->d_name[0] == '.' && clad->dir_showDotFiles == 0)
      continue;

    if(false /*symlink .... make connector?*/) {}
    else if(readableDir(name)) dirs.push_back(name);
    else addNode(name, Color("#0ff"), dirname, level, clad);

  }

  // Add readable directories to cladogram
  for(int i = 0; i < (int)dirs.size(); ++i) {

    name = dirs[i];
    string tuple = name.substr(name.rfind(folder_delimiter) + 1).substr(0,2);
    if(tuple == ".." || tuple == "." || tuple == "./")
      continue;

    addNode(name, Color("#00f"), dirname, level, clad);
    parseDir(name, clad, level + 1);

  }

  closedir(dir);

}

void ParserDIR::addNode(std::string name, Color color, std::string parent,
                        int level, Cladogram * clad) {
  Node * node = clad->addNode(name);
  node->color = color;
  node->parentName = parent;
  node->start = Date(level, 1);
  node->stop = node->start;
  node->iconfile = "";
  node->description = node->name;

}

bool readableDir(string dname) {
  DIR * dp;
  dp = opendir(dname.c_str());
  if(dp == NULL) return false;
  closedir(dp);
  return true;
}

DIR * new_indir(string dname) {
  DIR * dp;
  dp = opendir(dname.c_str());
  if(dp == NULL)
    throw "failed to open directory " + dname;
  return dp;
}
