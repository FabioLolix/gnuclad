/*
*  gnuclad.cpp - generate a cladogram
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
*
*
*  Usage: ./gnuclad --help
*/



#include "gnuclad.h"
#include "parser/csv.h"
#include "generator/csv.h"
#include "generator/svg.h"
#include "generator/conf.h"
#include "generator/png.h"

#include "../config.h"

#include <iostream>
#include <algorithm>
//~ #include <cstdlib>
//~ #include <cstring>
//~ #include <ctime>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
///
// Main
//

int main(int argc, char ** argv) {

  const string version = VERSION;
  string conffile = "";
  string inFormats = "csv";
  string outFormats = "csv, svg, conf";

  // Print version
  cout << "gnuclad " << version;

  string a1;
  if(argc > 1) a1 = argv[1];
  if(a1 == "-v" || a1 == "--version") {
    cout << "\n";
    return EXIT_SUCCESS;
  }

  // Print help
  string self = getBaseName(argv[0]);
  if( (argc != 3 && argc != 4) || a1 == "-h" || a1 == "--help" ) {

    cout << "\nUsage: " << self<<" INPUTFILE OUTPUT[FORMAT|FILE] [CONFIGFILE]\n"
         << " Example: " << self << " table.CSV SVG\n"
         << " Example: " << self << " Data.csv result.csv alternative.conf\n\n"
         << "Supported input formats: " << inFormats << '\n'
         << "Supported output formats: " << outFormats << "\n"
         << "Please consult the texinfo manual for in-depth explanations.\n\n";
    return EXIT_SUCCESS;

  }

  // Get input/output file information
  string source = a1;
  string dest = argv[2];
  string filename = getBaseName(source);
  string inputExt = getExt(source);  // lowercase extension
  string outputExt = getExt(dest);  // lowercase extension
  if(outputExt == "") {
    outputExt = strToLower(dest);
    dest = filename + "." + outputExt;
  }
  if(argc == 4) conffile = argv[3];

  // Chose parser
  Parser * parser = NULL;
  if     (inputExt == "csv") parser = new ParserCSV;
  else {
    cout << "\nError: unknown input file type: " << inputExt << '\n'
         << "Supported input formats: " << inFormats << '\n';
    exit(EXIT_FAILURE);
  }

  // Chose generator
  Generator * generator = NULL;
  if     (outputExt == "csv") generator = new GeneratorCSV;
  else if(outputExt == "svg") generator = new GeneratorSVG;
  else if(outputExt == "conf") generator = new GeneratorCONF;
  else if(outputExt == "png") generator = new GeneratorPNG;
  else {
    cout << "\nError: unknown output file type: " << outputExt << '\n'
         << "Supported output formats: " << outFormats << '\n';
    delete parser;
    exit(EXIT_FAILURE);
  }

  // Initialise Cladogram
  Cladogram * clad = NULL;  // split declare and init because of warnings
	clad = new Cladogram();
  clad->gnuclad_version = version;
  clad->inputFolder = getBaseFolder(source);

  int exitval = EXIT_FAILURE;
  cout << ": " << inputExt << " => " << outputExt;

  // The heavy lifting
  try {

    clad->parseOptions(conffile);

    InputFile in(source);
    parser->parseData(clad, in);
    safeClose(in.p);  // if we want to write to the same file

    clad->compute();

    OutputFile out(dest);
    generator->writeData(clad, out);

    exitval = EXIT_SUCCESS;

  } catch(const char * err) {
    cout << "\nError: " << err;
  } catch(string err) {
    cout << "\nError: " << err;
  } catch(exception e) {
    cout << "\nError: " << e.what();
  } catch(...) {
    cout << "\nError: unknown reason";
  }

  // Cleaning up
  delete clad;
  delete parser;
  delete generator;

  if(exitval == EXIT_FAILURE) {

    if(inputExt != outputExt) remove(dest.c_str());
    cout << "\nAborted\n";

  } else cout << "\nDone\n";

  return exitval;
}



////////////////////////////////////////////////////////////////////////////////
///
// Basic Class Implementations
//


Color::Color() {}
Color::Color(int tred, int tgreen, int tblue) {
  red = tred;
  green = tgreen;
  blue = tblue;

  if(red > 255 || green > 255 || blue > 255 ||
     red < 0   || green < 0   || blue < 0) {
    cout << "\nError: Color RGB to hex conversion failed!";
    throw 0;
  }

  std::string hred = rgb2hexHue(red);
  std::string hgreen = rgb2hexHue(green);
  std::string hblue = rgb2hexHue(blue);
  hex = hred + hgreen + hblue;
}
Color::Color(string thex) {
  hex = checkHexCol(thex);
  hex = hex.substr(1);

  int l = 0;
  if(hex.size() == 3) l = 1;
  else if(hex.size() == 6) l = 2;

  red = hex2rgbHue( hex.substr(0, l) );
  green = hex2rgbHue( hex.substr(l, l) );
  blue = hex2rgbHue( hex.substr(2*l, l) );

  if(red > 255 || green > 255 || blue > 255 ||
     red < 0   || green < 0   || blue < 0) {
    cout << "\nError: Color RGB to hex conversion failed!";
    throw 0;
  }
}

Date::Date(int tyear, int tmonth, int tday) {
  year = tyear;
  month = tmonth;
  day = tday;
  monthset = false;
  dayset = false;
  if(month != 0) monthset = true;
  if(day != 0) dayset = true;
}
Date::Date(int tyear, int tmonth) {
  *this = Date(tyear, tmonth, 0);
}
Date::Date(int tyear) {
  *this = Date(tyear, 0, 0);
}
Date::Date() {
  *this = Date(0, 0, 0);
}
Date::Date(const std::string str) {
  year = 0; month = 0; day = 0;
  monthset = false; dayset = false;
  vector<string> d;
  explode(str, '.', &d);
  if(d.size() == 3) {
    day = str2int(d.at(2));
    dayset = true;
  }
  if(d.size() >= 2) {
    month = str2int(d.at(1));
    monthset = true;
  }
  if(d.size() >= 1) year = str2int(d.at(0));
}
bool Date::operator<(const Date d) {
  if(year < d.year ||
    (year == d.year && month < d.month) ||
    (year == d.year && month == d.month && day < d.day) )
    return true;
  else return false;
}
bool Date::operator==(Date d) {
  return ( !(*this < d)&& !(d < *this) );
}
Date Date::operator+(const Date d) {
  return Date(d.year + year, d.month + month, d.day + day);
}

NameChange::NameChange() {}
NameChange::NameChange(std::string tnewName, Date tdate,
                       std::string tdescription) {
  newName = tnewName;
  date = tdate;
  description = tdescription;
}

Image::Image() {
  x = 0;
  y = 0;
}

Domain::Domain() {
  intensity = 50;
  node = NULL;
}

Node::Node() {
  offset = 0;
  size = 1;
  parent = NULL;
}

void Node::addNameChange(std::string newName, Date date,
                         std::string description) {
  nameChanges.push_back( NameChange(newName, date, description) );
}

Node * Node::root() {
  Node * r = this;
  while(r->parent != NULL) r = r->parent;
  return r;
}

bool Node::derivesFrom(Node * p) {
  Node * n = this;
  while(n->parent != NULL) {
    if(p == n->parent) return true;
    n = n->parent;
  }
  return false;
}

Connector::Connector() {
  from = NULL;
  to = NULL;
}

InputFile::InputFile(std::string tname) {
  name = tname;
  p = new_infile(name);
}
InputFile::~InputFile() {
  safeClose(p);
  delete p;
}

OutputFile::OutputFile(std::string tname) {
  name = tname;
  p = new_outfile(name);
}
OutputFile::~OutputFile() {
  safeClose(p);
  delete p;
}



////////////////////////////////////////////////////////////////////////////////
///
// Parser/Generator destructor, required by some gcc versions
//

Parser::~Parser() {}
Generator::~Generator() {}
