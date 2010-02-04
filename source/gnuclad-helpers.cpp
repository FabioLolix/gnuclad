/*
*  gnuclad-helpers.cpp - implements helper functions for gnuclad
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

#include "gnuclad.h"

#include <iostream>
#include <limits>

using namespace std;

// Requires the user to press ENTER before continuing - for debugging
void pressEnter() {
  std::cout << "\nPress ENTER to continue...\n";
  cin.clear();
  cin.sync();
  std::cin.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );
}

// Returns a lowercase version of the input string
string strToLower(string str) {
  for(int i = 0; i < (int)str.length(); ++i)
    str[i] = tolower(str[i]);
  return str;
}

// Returns the part of the string up to the last dot
//         the input string if there is no dot
string getBaseName(string fname) {
  fname = fname.substr(fname.rfind('/') + 1);
  if(fname.rfind('.') != std::string::npos)
    return fname.substr(0, fname.rfind('.'));
  else
    return fname;
}

// Returns the part of a string after the last dot
//         an empty if there is no dot
string getExt(string fname) {
  if(fname.rfind('.') != std::string::npos)
    return strToLower(fname.substr(fname.rfind('.') + 1));
  else
    return "";
}

// Returns a new input file, which you have to delete!
ifstream * new_infile(const string fname) {
  ifstream * fp = new ifstream;
  fp->open(fname.c_str());
  if( fp->is_open() ) return fp;
  throw "failed to open file " + fname;
}

// Returns a new output file, which you have to delete!
ofstream * new_outfile(const string fname) {
  ofstream * fp = new ofstream;
  fp->open(fname.c_str());
  if( fp->is_open() ) return fp;
  throw "failed to open file " + fname;
}

void safeClose(std::ifstream * fp) {
  if(fp != NULL && fp->is_open())
	  fp->close();
}

void safeClose(std::ofstream * fp) {
  if(fp != NULL && fp->is_open()) {
    fp->flush();
	  fp->close();
  }
}

// Explode function
void explode(const string str, const char delimiter, vector<string> * v) {
  string buff = "";
  for(int i = 0; i < (int)str.length(); ++i) {
    if(str[i] == delimiter) {
      v->push_back(buff);
      buff = "";
    } else buff += str[i];
  }
  if(buff != "") v->push_back(buff);
}

// Explode, but not within the "safe" sequence
// Attention: Fails if the "safe" sequence contains the toggle itself and
// then the delimiter right behind
// Example 1: explodeSafely("abc|*dexf|ghi*|jkl", '|', '*', ...)
// Example 2: explodeSafely("abc|*de*f|ghi*|jkl", '|', '*', ...)
// Example 3: explodeSafely("abc|*def*|ghi*|jkl", '|', '*', ...)
// Result 1: [abc] [*dexf|ghi*] [jkl]
// Result 2: [abc] [*de*f|ghi*] [jkl]
// Result 3: [abc] [*def*] [ghi*|jkl]
void explodeSafely(const string str, const char delimiter, const char toggle,
                   vector<string> * v) {
  string buff = "";
  bool t = false;
  int len = (int)str.length();
  for(int i = 0; i < len; ++i) {

    if(str[i] == toggle) {  // only toggle off if we have delimiter after toggle
      if(t == false) t = true;
      if(t == true && i < len-1 && str[i+1] == delimiter) t = false;
    }
    if(str[i] == delimiter && t == false) {
      v->push_back(buff);
      buff = "";
    } else buff += str[i];

  }
  if(buff != "") v->push_back(buff);
}


// Find and replace all occurrences of the string "find" in the string "str"
// with the string "replace", then return "str"
std::string findReplace(std::string str, std::string find, std::string replace){

  if(find == replace)
    throw "findReplace: find parameter == replace parameter";
  string::size_type pos = 0;
  while( (pos = str.find(find, pos)) != string::npos )
    str.replace(pos, find.size(), replace);
  return str;

}

// Converts a string to a double
double str2double(const std::string str) {
  std::istringstream ss (str);
  double d;
  ss >> d;
  if(ss.fail() == true) {
    cout << "\nError: string to double conversion failed!";
    throw 0;
  }
  return d;
}

// Converts a string to an integer
int str2int(const std::string str) {
  std::istringstream ss (str);
  int n;
  ss >> n;
  if(ss.fail() == true) {
    cout << "\nError: string to int conversion failed!";
    throw 0;
  }
  return n;
}

// Converts an integer to a string
std::string int2str(const int n) {
  std::ostringstream ss;
  ss << n;
  if(ss.fail() == true) {
    cout << "\nError: int to string conversion failed!";
    throw 0;
  }
  return ss.str();
}

// Returns Date object with the time at execution
Date currentDate() {
  tm * p = NULL;
  time_t t = time(NULL);
  p = gmtime(&t);
  return Date(p->tm_year + 1900, p->tm_mon + 1, p->tm_mday);
}

// Returns a string yyyy.mm.dd based on input Date
std::string Date2str(Date d) {
  return int2str(d.year) + "." + int2str(d.month) + "." + int2str(d.day);
}

// Returns pixel offset based on Date object
int datePX(Date d, const Cladogram * clad) {
  int yrPX = clad->yearPX;
  float monPX = yrPX / float(clad->monthsInYear);
  float dayPX = yrPX / float(clad->monthsInYear * clad->daysInMonth);

  if(d.month > 0) d.month -= 1;
  if(d.day > 0) d.day -= 1;

  return (d.year - clad->beginningOfTime.year) * yrPX
          + int(d.month * monPX) + int(d.day * dayPX);
}

// Returns the supplied color string if it is in a valid hex color format
std::string checkHexCol(const std::string color) {
  if(color[0] != '#' ||
    (color != "none" && color.size() != 4 && color.size() != 7 )) {
      cout << "\nError: Color is expected to have either "
           << "the format #abc or #abcdef"
           << "\n       none is also valid";
      throw 0;
  }
  return color;
}
