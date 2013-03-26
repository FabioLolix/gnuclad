/*
*  dir.cpp - implements a directory parser for gnuclad
*
*  Copyright (C) 2010-2013 Donjan Rodic <donjan@dyx.ch>
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
#include <sys/stat.h>
#include <iostream>

using namespace std;


ParserDIR::ParserDIR() {}
ParserDIR::~ParserDIR() {}

void ParserDIR::parseData(Cladogram * clad, InputFile & in) {

  clad->beginningOfTime = Date(1);
  clad->endOfTime = Date(1);
  clad->truncateFolder = true;
  clad->inVitro = true;  // just surpressing warnings
  clad->tighterDomains = true;
  clad->treeSpacingBiggerThan = 0;

  if(clad->derivType != 1 && clad->derivType != 5)
    cout << "\nWARNING: derivType 1 or 5 recommended";
  clad->stopFadeOutPX = 0;
  clad->rulerMonthWidth = 0;

  string dir = in.name;

  // remove trailing folder_delimiter
  if(dir.substr(dir.size()-1) == folder_delimiter)
    dir = dir.substr(0, dir.size()-1);

  addNode(dir, clad->dir_colorDir, "", 0, clad);
  parseDir(dir, clad, 1);

  // Fix trailing year
  clad->endOfTime.year--;
  clad->endOfTime.month = clad->monthsInYear;
  clad->endOfTime.day = clad->daysInMonth;
}


void ParserDIR::parseDir(std::string dirname, Cladogram * clad, int level) {

  if(clad->endOfTime.year <= level) clad->endOfTime.year = level + 1;

  string name;
  DIR * dir = new_indir(dirname);
  struct dirent * dirElem;
  int domcount = 0;  // domain counter

  // Readable directories
  vector<string> dirs;

  // Scan all nodes in current folder
  while ((dirElem = readdir(dir)) != NULL) {

    name = dirname + folder_delimiter + dirElem->d_name;
    if(dirElem->d_name[0] == '.' && clad->dir_showDotFiles == 0)
      continue;

    if (islink(dirElem))
      addNode(name, clad->dir_colorLink, dirname, level, clad);

    else if(readableDir(name)) {
      string t = dirElem->d_name;
      t = t.substr(0,2);
      if(t != ".." && t != "." && t != "./" && t != ".\\")
        dirs.push_back(name);
    }

    else addNode(name, clad->dir_colorFile, dirname, level, clad);

    // Add domains
    string t = dirElem->d_name;
    t = t.substr(0,2);
    if(t != ".." && t != "." && t != "./" && t != ".\\") ++domcount;
    if(level > 1 && clad->dir_domainSize>0 && domcount == clad->dir_domainSize){
      Domain * d = clad->addDomain(dirname);
      d->color = clad->dir_colorDir;
      d->intensity = clad->dir_domainIntensity;
    }
  }

  // Add readable directories to cladogram
  for(int i = 0; i < (int)dirs.size(); ++i) {
    addNode(dirs[i], clad->dir_colorDir, dirname, level, clad);
    parseDir(dirs[i], clad, level + 1);
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

  // First part, check if it's a directory
  struct stat filestat;

  if (stat(dname.c_str(), &filestat) == -1) {
    cout << "\n" << dname;
    perror("stat");
  }
  if(!S_ISDIR(filestat.st_mode))
    return false;

  // Second part, makes sure we can actually read it... but slows things down
  //~ DIR * dp;
  //~ dp = opendir(dname.c_str());
  //~ if(dp == NULL) return false;
  //~ closedir(dp);

  return true;

}

DIR * new_indir(string dname) {
  DIR * dp;
  dp = opendir(dname.c_str());
  if(dp == NULL)
    throw "failed to open directory " + dname;
  return dp;
}
