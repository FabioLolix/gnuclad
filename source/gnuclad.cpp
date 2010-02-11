/*
*  gnuclad.cpp - generate a cladogram
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
*
*
*  Usage: ./gnuclad INPUTFILE OUTPUTFORMAT [CONFIGFILE]
*/



#include "gnuclad.h"
#include "parserCSV.h"
#include "generatorCSV.h"
#include "generatorSVG.h"
#include "generatorPNG.h"

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

  string version = "ALPHA";
  string conffile = "";
  string inFormats = "csv";
  string outFormats = "csv, svg";

  // Version and Help
  string self = argv[0], a1;
  if(self.substr(0, 2) == "./") self.replace(0, 2, "");

  cout << "gnuclad v" << version;

  if(argc > 1) a1 = argv[1];
  if(a1 == "-v" || a1 == "--version") {
    cout << "\n";
    return EXIT_SUCCESS;
  }

  if( (argc != 3 && argc != 4) || a1 == "-h" || a1 == "--help" ) {

    cout << "\nUsage: " << self<<" INPUTFILE OUTPUT[FORMAT|FILE] [CONFIGFILE]\n"
         << " Example: " << self << " table.CSV SVG\n"
         << " Example: " << self << " Data.csv result.csv alternative.conf\n"
         << "Supported input formats: " << inFormats << '\n'
         << "Supported output formats: " << outFormats << "\n\n";
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
  else if(outputExt == "png") generator = new GeneratorPNG;
  else {
    cout << "\nError: unknown output file type: " << outputExt << '\n'
         << "Supported output formats: " << outFormats << '\n';
    delete parser;
    exit(EXIT_FAILURE);
  }

  // Declare resources
  Cladogram * clad = NULL;
  clad = new Cladogram();
  ifstream * infile = NULL;
  ofstream * outfile = NULL;
  int exitval = EXIT_SUCCESS;

  cout << ": " << inputExt << " => " << outputExt;

  // The heavy lifting
  try {

    clad->parseOptions(conffile);

    // if(isFolder(source)) todo
    infile = new_infile(source);
    parser->parseData(clad, infile);
    safeClose(infile);  // if we want to write to the same file

    clad->compute();

    outfile = new_outfile(dest);
    generator->writeData(clad, outfile);

  } catch(const char * err) {
    cout << "\nError: " << err;
    exitval = EXIT_FAILURE;
  } catch(string err) {
    cout << "\nError: " << err;
    exitval = EXIT_FAILURE;
  } catch(exception e) {
    cout << "\nError: " << e.what();
    exitval = EXIT_FAILURE;
  } catch(...) {
    cout << "\nError: unknown reason";
    exitval = EXIT_FAILURE;
  }

  // Cleaning up
  delete clad;
  delete parser;
  delete generator;
  safeClose(infile);
  safeClose(outfile);
  delete infile;
  delete outfile;  // deleting NULL pointers is safe! (in case new_infile fails)

  if(exitval == EXIT_FAILURE) {

    cout << "\nAborted\n";
    if(inputExt != outputExt)
      remove(dest.c_str());

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

Date::Date() {
  year = 0; month = 0; day = 0;
  monthset = false; dayset = false;
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



////////////////////////////////////////////////////////////////////////////////
///
// Cladogram Class Implementation
//

Cladogram::Cladogram() {

  maximumOffset = 0;
  endOfTime = currentDate();  // can be changed as well...
  beginningOfTime = endOfTime;


  // the following can be overwritten in the config file:

  infoBoxTitle = "Title";
  infoBoxTitleSize = 24;
  //~ infoBoxText.push_back("Lorem ipsum dolor sit amet");
  infoBoxTextSize = 16;
  infoBoxFont = "Arial";
  infoBoxFontColor = Color("#000");
  infoBoxColor1 = Color("#5bf");
  infoBoxColor2 = Color("#38f");
  infoBoxX = 10;
  infoBoxY = 50;
  infoBoxWidth = 50;
  infoBoxHeight = 50;

  fontCorrectionFactor = 1.0;
  orientation = 1;
  treeMode = 0;
  sortKey = 0;
  optimise = 99;
  treeSpacing = 1;
  treeSpacingBiggerThan = 5;

  mainBackground = Color("#fff");
  rulerWidth = 2;
  rulerColor = Color("#ddd");
  rulerMonthWidth = 1;
  rulerMonthColor = Color("#eee");

  lineWidth = 2;
  offsetPX = 20;
  stopFadeOutPX = 30;
  stopSpacing = Date(0, 2, 0);
  yearPX = 100;

  labelFont = "Arial";
  labelFontSize = 16;
  labelFontColor = Color("#000");
  labelBGOpacity = 0;
  nameChangeType = 0;

  derivType = 0;
  dotRadius = 10;
  smallDotRadius = 5;
  dotType = 0;

  connectorDots = 1;
  connectorsDashed = 1;

  yearLinePX = 40;
  yearLineColor1 = Color("#037");
  yearLineColor2 = Color("#37d");
  yearLineFont = "Arial";
  yearLineFontSize = 28;
  yearLineFontColor = Color("#fff");

  daysInMonth = 30;
  monthsInYear = 12;

  descriptionIsHyperLink = 0;

  debug = 0;

}

Cladogram::~Cladogram() {
  for(int i = 0; i < (int)nodes.size(); ++i)
    if(nodes.at(i) != NULL) delete nodes.at(i);
  for(int i = 0; i < (int)connectors.size(); ++i)
    if(connectors.at(i) != NULL) delete connectors.at(i);
  for(int i = 0; i < (int)domains.size(); ++i)
    if(domains.at(i) != NULL) delete domains.at(i);

  for(int i = 0; i < (int)includeSVG.size(); ++i)
    if(includeSVG.at(i) != NULL) delete includeSVG.at(i);
  for(int i = 0; i < (int)includePNG.size(); ++i)
    if(includePNG.at(i) != NULL) delete includePNG.at(i);
}


void Cladogram::parseOptions(const string filename) {

  ifstream fp(filename.c_str());
  string bla = "";

  if(filename != "") bla = "(" + filename + ") found";
  else bla = "specified";
  if( !(fp.is_open()) ) {
    cout << "\nNo config file " << bla << ". Using default options.";
    return;
  }

  string line, opt, val;
  int p = 0;

  while( !fp.eof() && fp.good() ) {

    getline(fp, line);

    if(line == "") continue;
    if(line[0] == '#') continue;

    p = line.find("=");
    opt = line.substr(0, p);
    val = line.substr(p + 1, line.size());

    // strip trailing and leading whitespaces, and quotes
    if(opt.size() > 0) {
      while(opt[0] == ' ') opt = opt.substr(1);
      while(opt[opt.size()-1] == ' ') opt = opt.substr(0, opt.size()-1);
    }
    if(val.size() > 0) {
      while(val[0] == ' ') val = val.substr(1);
      while(val[val.size()-1] == ' ') val = val.substr(0, val.size()-1);
      while(val[0] == '"' || val[0] == '\'') val = val.substr(1);
      while(val[val.size()-1] == '"' || val[val.size()-1] == '\'')
        val = val.substr(0, val.size()-1);
    }

    try {

      if     (opt == "infoBoxTitle") infoBoxTitle = val;
      else if(opt == "infoBoxTitleSize") infoBoxTitleSize = str2int(val);
      else if(opt == "infoBoxText") infoBoxText.push_back(val);
      else if(opt == "infoBoxTextSize") infoBoxTextSize = str2int(val);
      else if(opt == "infoBoxFont") infoBoxFont = val;
      else if(opt == "infoBoxFontColor") infoBoxFontColor = Color(val);
      else if(opt == "infoBoxColor1") infoBoxColor1 = Color(val);
      else if(opt == "infoBoxColor2") infoBoxColor2 = Color(val);
      else if(opt == "infoBoxX") infoBoxX = str2int(val);
      else if(opt == "infoBoxY") infoBoxY = str2int(val);
      else if(opt == "infoBoxWidth") infoBoxWidth = str2int(val);
      else if(opt == "infoBoxHeight") infoBoxHeight = str2int(val);
      else if(opt == "fontCorrectionFactor")fontCorrectionFactor=str2double(val);
      else if(opt == "orientation") orientation = str2int(val);
      else if(opt == "treeMode") treeMode = str2int(val);
      else if(opt == "sortKey") sortKey = str2int(val);
      else if(opt == "optimise") optimise = str2int(val);
      else if(opt == "treeSpacing") treeSpacing = str2int(val);
      else if(opt == "treeSpacingBiggerThan")treeSpacingBiggerThan=str2int(val);
      else if(opt == "mainBackground") mainBackground = Color(val);
      else if(opt == "rulerWidth") rulerWidth = str2int(val);
      else if(opt == "rulerColor") rulerColor = Color(val);
      else if(opt == "rulerMonthWidth") rulerMonthWidth = str2int(val);
      else if(opt == "rulerMonthColor") rulerMonthColor = Color(val);
      else if(opt == "lineWidth") lineWidth = str2int(val);
      else if(opt == "offsetPX") offsetPX = str2int(val);
      else if(opt == "stopFadeOutPX") stopFadeOutPX = str2int(val);
      else if(opt == "stopSpacing") stopSpacing = Date(val);
      else if(opt == "yearPX") yearPX = str2int(val);
      else if(opt == "labelFont") labelFont = val;
      else if(opt == "labelFontSize") labelFontSize = str2int(val);
      else if(opt == "labelFontColor") labelFontColor = Color(val);
      else if(opt == "labelBGOpacity")labelBGOpacity = str2int(val);
      else if(opt == "nameChangeType") nameChangeType = str2int(val);
      else if(opt == "derivType") derivType = str2int(val);
      else if(opt == "dotRadius") dotRadius = str2int(val);
      else if(opt == "smallDotRadius") smallDotRadius = str2int(val);
      else if(opt == "dotType") dotType = str2int(val);
      else if(opt == "connectorDots") connectorDots = str2int(val);
      else if(opt == "connectorsDashed") connectorsDashed = str2int(val);
      else if(opt == "yearLinePX") yearLinePX = str2int(val);
      else if(opt == "yearLineColor1") yearLineColor1 = Color(val);
      else if(opt == "yearLineColor2") yearLineColor2 = Color(val);
      else if(opt == "yearLineFont") yearLineFont = val;
      else if(opt == "yearLineFontSize") yearLineFontSize = str2int(val);
      else if(opt == "yearLineFontColor") yearLineFontColor = Color(val);
      else if(opt == "daysInMonth") daysInMonth = str2int(val);
      else if(opt == "monthsInYear") monthsInYear = str2int(val);
      else if(opt == "endOfTime") endOfTime = Date(val);
      else if(opt == "descriptionIsHyperLink")
        descriptionIsHyperLink = str2int(val);
      else if(opt == "debug") debug = str2int(val);
      else cout << "\nIGNORING unrecognised config option: " << opt;

    } catch (...) {
      throw "invalid config setting for option: " + opt;
    }

    if(debug > 0) cout << "\nDEBUG " << opt << " = " << val;

  }

  fp.close();

}

Node * Cladogram::addNode(std::string tname) {
  Node * node = new Node;
  node->name = tname;
  nodes.push_back(node);
  return node;
}

Domain * Cladogram::addDomain(std::string tname) {
  Domain * domain = new Domain;
  domain->nodeName = tname;
  domains.push_back(domain);
  return domain;
}

Connector * Cladogram::addConnector() {
  Connector * c = new Connector;
  connectors.push_back(c);
  return c;
}

Image * Cladogram::addImage(std::string tname, std::vector<Image *> &vector) {
  Image * i = new Image;
  i->filename = tname;
  vector.push_back(i);
  return i;
}

// Returns the date with resolved overflow
// Use only if really needed (for comparisons), because you usually don't want
// to equalise 2000.1.31 with 2000.2.1 if your daysInMonth is 30
Date Cladogram::rOf(Date d) {
  while(d.day > daysInMonth) {
    d.month += 1;
    d.day -= daysInMonth;
  }
  while(d.month > monthsInYear) {
    d.year += 1;
    d.month -= monthsInYear;
  }
  return d;
}

void Cladogram::compute() {

  int nCount = (int)nodes.size();
  int dCount = (int)domains.size();
  int cCount = (int)connectors.size();

  cout << "\nComputing cladogram for " << nCount << " nodes, "
                                       << cCount << " connectors and "
                                       << dCount << " domains...";

  if     (sortKey == 0) {}
  else if(sortKey == 1) stable_sort(nodes.begin(), nodes.end(), compareName());
  else if(sortKey == 2) stable_sort(nodes.begin(), nodes.end(), compareDate());

  Node * n = NULL;
  Node * r = NULL;
  Domain * d = NULL;
  Connector * c = NULL;

  // Basics
  // Juggle dates, warn for duplicates and assing parent pointers
  roots.clear();
  std::vector<int> toErase;
  for(int i = 0; i < nCount; ++i) {

    n = nodes.at(i);

    // If "start" is newer than endOfTime, ignore that node
    if(endOfTime < n->start) {
      cout << "\nIgnoring " << n->name << " (entry " << i+1
           << "): starts after specified End Of Time";
      toErase.push_back(i);
      continue;
    }

    // If "stop" is newer than endOfTime, set "stop" to endOfTime
    if(endOfTime < n->stop)
      n->stop = endOfTime;

    // If no "stop" time specified, set it to endOfTime
    if(n->stop.year == 0 && n->stop.month == 0 && n->stop.day == 0)
      n->stop = endOfTime;

    // Cosmetic improvement, since we have no 0th month or day
    if(n->start.month == 0) n->start.month = 1;
    if(n->start.day == 0) n->start.day = 1;
    if(n->stop.month == 0) n->stop.month = 1;
    if(n->stop.day == 0) n->stop.day = 1;

    // "stop" must not occur before "start
    if(n->stop < n->start)
      throw n->name + " stops before it starts";

    // Get earliest time
    if(n->start < beginningOfTime)
      beginningOfTime = n->start;

    // Find pointer to parent and check for duplicates
    string parName = n->parentName;
    if(n->name == parName)
      throw n->name + " has the same name as it's parent";
    if(parName == "") roots.push_back(n);
    for(int j = 0; j < nCount; ++j) {

      if(j > i && n->name == nodes.at(j)->name)  // might result in bad children
        cout << "\nWarning: " << n->name
             << " (entry " << j+1 << ") is already listed at position " << i+1;

      if(parName == nodes.at(j)->name)
        n->parent = nodes.at(j);

    }
    if(parName != "" && n->parent == NULL)
      throw "unable to find parent (" + parName +") for " + n->name;

    // Throw if node starts before parent and add node to parent's children
    if(n->parent != NULL) {

      if(n->start < n->parent->start)
        throw n->name + " starts before it's parent " + n->parent->name;

      if(n->parent->stop < n->start)
        cout << "\nWarning: " << n->name
             << " starts AFTER it's parent (" << n->parent->name << ") stops!";

      n->parent->children.push_back(n);

    }
  }

  // Assign nodes to domains
  for(int i = 0; i < dCount; ++i) {
    d = domains.at(i);
    for(int j = 0; j < nCount; ++j)
      if(nodes.at(j)->name == d->nodeName)
        d->node = nodes.at(j);
    if(d->node == NULL)
      throw "unable to assign domain to " + d->nodeName;

    if(endOfTime < d->node->start) {
      domains.erase(domains.begin() + i);
      --dCount;
      --i;
    }
  }

  // Assign nodes to connectors
  for(int i = 0; i < cCount; ++i) {

    c = connectors.at(i);

    // Cosmetic improvement, since we have no 0th month or day
    if(c->fromWhen.month == 0) c->fromWhen.month = 1;
    if(c->fromWhen.day == 0) c->fromWhen.day = 1;
    if(c->toWhen.month == 0) c->toWhen.month = 1;
    if(c->toWhen.day == 0) c->toWhen.day = 1;

    if(endOfTime < c->fromWhen || endOfTime < c->toWhen) {
      cout << "\nIGNORING connector " << c->fromName << " -> " << c->toName
           << " (starting " << c->fromWhen.year<< "." << c->fromWhen.month
           << "." << c->fromWhen.day << " stopping " << c->toWhen.year << "."
           << c->toWhen.month << "." << c->toWhen.day
           << "): reaches after specified End Of Time";
      connectors.erase(connectors.begin() + i);
      --cCount;
      --i;
      continue;
    }

    for(int j = 0; j < nCount; ++j) {
      if(nodes.at(j)->name == c->fromName)
        c->from = nodes.at(j);
      if(nodes.at(j)->name == c->toName)
        c->to = nodes.at(j);
    }
    if(c->from == NULL || c->to == NULL)
      throw "unable to assign connector " + c->fromName + " -> " + c->toName;

  }

  // Erase ignored nodes
  nCount -= (int)toErase.size();
  for(int i = (int)toErase.size() - 1; i >= 0; --i)
    nodes.erase(nodes.begin() + toErase.at(i));
  int rCount = (int)roots.size();

  // Push through size
  // Requires full parent paths, hence a new pass
  for(int i = 0; i < nCount; ++i) {
    n = nodes.at(i);
    while(n->parent != NULL) {
      n->parent->size += 1;
      n = n->parent;
    }
  }

  // Build the map
  // Assign offsets to all nodes
  // Requires correct size on all nodes, hence a new pass
  int offset = 0;
  for(int i = 0; i < rCount; ++i) {

    int treeOffset = 0;
    r = roots.at(i);
    std::deque<Node *> nodeTree;
    nodeTree.push_back(r);

    // Expand the tree if the current root size is bigger than 1
    while((int)nodeTree.size() < r->size) {

      n = nodeTree.at(treeOffset);
      if(n->offset == -1) {
        ++treeOffset;
        continue;
      }

      // Mark current node as "processed" by setting it's offset to -1
      n->offset = -1;

      stable_sort(n->children.begin(), n->children.end(), compareDate());

      if(treeMode == 0)
        compute_subtreeBoth(nodeTree, treeOffset, n);
      else if(treeMode == 1)
        compute_subtreeLower(nodeTree, treeOffset, n);
      else if(treeMode == 2)
        compute_subtreeLowerInverse(nodeTree, treeOffset, n);

      //TODO: maybe optimise here with both-rest-subtree swapping

    }

    if(r->size != (int)nodeTree.size())
      throw "something went wrong with the subtree calculation";

    // Assign offsets according to the tree
    // Will only have one assignment if it's a single root
    for(int j = 0; j < (int)nodeTree.size(); ++j)
      nodeTree.at(j)->offset = offset + j;

    offset += r->size;

  }

  // Optimisation: queue single nodes and squash trees
  // Optmisation will fail (moveTo() and tree optimisations) if the vectors are
  // not sorted by offset.
  // Sorting shouldn't be needed if the building pass did everything correctly,
  // but we might alter the way we build the map in the future.
  if(optimise > 0) {
    stable_sort(nodes.begin(), nodes.end(), compareOffset());
    stable_sort(roots.begin(), roots.end(), compareOffset());
  }
  for(int i = 0; i < rCount; ++i) {

    if(optimise == 0) break;
    r = roots.at(i);

    if(r->size == 1) {  // Single root

      int opt = optimise/10;

      // Don't reach behind trees of certain size
      int upTo = rCount;
      int j = i+1;
      while(j < rCount) {
        if(opt <= 1) { ++j; break; }  // look only at next root
        if(opt <= 2 && roots.at(j)->size > 1) break;
        if(opt <= 5 && roots.at(j)->size > 5) break;
        if(opt <= 6 && roots.at(j)->size > 10) break;
        if(opt <= 7 && roots.at(j)->size > 20) break;
        ++j;
      }
      upTo = j;

      optimise_injectSingleRootAt(i, upTo);

    } else {  // Tree root

      int opt = optimise%10;

      // Get first and last node positions of current continuous root tree block
      int first = 0;
      while(first < nCount && nodes.at(first)->root() != r) ++first;
      int last = first;
      while(last < nCount && nodes.at(last)->root() == r) ++last;

      optimise_nextTree(first, last);
      //~ if(opt >= 5) {
        //~ optimise_interleaveTree(first, last);                                <= pseudocode
        //~ if(opt >= 8)
          //~ optimise_interleaveAllRoots(first, last);
      //~ }

      if(opt >= 3)
        optimise_pullTree(first, last);

    }
  }

  // Insert spacing at tree edges
  for(int i = 0; i < nCount - 1; ++i) {
    Node * a = nodes.at(i);
    Node * b = nodes.at(i+1);
    if( a->root() != b->root() )  // if not in same tree
      if(a->parent != NULL || b->parent != NULL ||  // if not root nodes
         a->children.size() > 0 || b->children.size() > 0)  // or have children
        if(a->root()->size > treeSpacingBiggerThan ||  // honor option
           b->root()->size > treeSpacingBiggerThan)
          moveOffsetsHigherThan(a->offset, treeSpacing);
  }

  // Set domain offsets
  for(int i = 0; i < dCount; ++i) {
    d = domains.at(i);

    // Get minimum and maximum offsets of children this domain's node has
    int min = nCount-1, max = 0;
    for(int j = 0; j < nCount; ++j) {
      n = nodes.at(j);
      if(n->derivesFrom(d->node)) {
        if(n->offset < nodes.at(min)->offset)
          min = j;
        if(n->offset > nodes.at(max)->offset)
          max = j;
      }
    }

    d->offsetA = nodes.at(min)->offset;
    d->offsetB = nodes.at(max)->offset;
    if(d->offsetA > d->node->offset) d->offsetA = d->node->offset;
    if(d->offsetB < d->node->offset) d->offsetB = d->node->offset;
  }

  // Set connector offsets
  for(int i = 0; i < cCount; ++i) {
    c = connectors.at(i);
    c->offsetA = c->from->offset;
    c->offsetB = c->to->offset;
    //~ if(c->offsetA > c->offsetB)   // this looks bad with dashed connectors
      //~ swap(c->offsetA, c->offsetB);
  }

  // Get total size (the maximum offset)
  for(int i = 0; i < nCount; ++i)
    if(maximumOffset < nodes.at(i)->offset)
      maximumOffset = nodes.at(i)->offset;


  if(debug > 0) debug_cladogram_compute();

}


// Prints some debug info
void Cladogram::debug_cladogram_compute() {

  for(int i = 0; i < (int)nodes.size(); ++i) {
    Node * n = nodes.at(i);
    string rt = (n->parent == NULL)?(" *"):("");
    cout << "\nDEBUG\t" << n->name << rt
         << "    \t  \tsize: " << n->size << "    \toffset: " << n->offset;
  }
  cout << "\n";
  for(int i = 0; i < (int)connectors.size(); ++i) {
    Connector * c = connectors.at(i);
    cout << "\nDEBUG\t" << c->fromName << "  \t=>\t" << c->toName
         << "   \t(" << c->offsetA << ","<< c->offsetB << ")";
  }
  cout << "\n";
  for(int i = 0; i < (int)domains.size(); ++i) {
    Domain * d = domains.at(i);
    cout << "\nDEBUG\t domain " << d->nodeName
         << "    \t(" << d->offsetA << "," << d->offsetB << ")";
  }
  cout << "\n\nDEBUG\tFrom "
       << beginningOfTime.year << "." << beginningOfTime.month << "."
       << beginningOfTime.day << " to "
       << endOfTime.year << "." << endOfTime.month << "." << endOfTime.day
       << "\ttotal size: " << maximumOffset + 1 << "\n";
  cout.flush();

}

// Generates a treeMap with the children distributed on both sides
void Cladogram::compute_subtreeBoth(std::deque<Node *> &tree,
                                    int pos, Node * n) {

  int childCount = (int)n->children.size();
                                                                                        ///TODO: OPTIMISE HERE??
  // optimise: if i node (has no children? and) ends before i+1 node starts, put them on same side


  // Insert the upper subtree before current node
  for(int i = 1; i < childCount; i += 2)
    tree.insert(tree.begin() + pos + i/2, n->children.at(i));

  // Insert the lower subtree after current node in reverse order
  for(int i = 0; i < childCount; i += 2)
    tree.insert(tree.begin() + pos + (childCount/2) + 1, n->children.at(i));

}

// Generates a treeMap with the children only below the parent
void Cladogram::compute_subtreeLower(std::deque<Node *> &tree,
                                    int pos, Node * n) {

  // Insert all child nodes after current node in reverse order
  for(int i = 0; i < (int)n->children.size(); ++i)
    tree.insert( tree.begin() + pos + 1, n->children.at(i) );

}

// Generates a treeMap with the children only below the parent - inverse order
void Cladogram::compute_subtreeLowerInverse(std::deque<Node *> &tree,
                                    int pos, Node * n) {

  // Insert all child nodes after current node
  for(int i = 0; i < (int)n->children.size(); ++i)
    tree.insert( tree.begin() + pos + 1 + i, n->children.at(i) );

}

// Returns true if specified node fits into given offset
// without overlapping other nodes in that offset.
// False otherwise.
bool Cladogram::fitsInto(const int offset, Node * node) {

  vector<Node *> tmp;
  for(int i = 0; i < (int)nodes.size(); ++i)
    if(nodes.at(i)->offset == offset)
      tmp.push_back(nodes.at(i));

  if(offset == node->offset) return false;
  if(tmp.size() == 0) return true;

  sort(tmp.begin(), tmp.end(), compareDate());

  // Compare node to first and last in tmp
  if( rOf(node->stop + stopSpacing) < tmp.at(0)->start ||
      rOf(tmp.at(tmp.size()-1)->stop + stopSpacing) < node->start )
    return true;
  // Compare node to all others in tmp
  for(int i = 0; i < (int)tmp.size() - 1; ++i)
    if( rOf(tmp.at(i)->stop + stopSpacing) < node->start &&
        rOf(node->stop + stopSpacing) < tmp.at(i+1)->start )
      return true;

  return false;
}

// Move all nodes with an offset higher than the specified one by given value.
void Cladogram::moveOffsetsHigherThan(const int offset, const int move) {
  for(int i = 0; i < (int)nodes.size(); ++i)
    if(nodes.at(i)->offset > offset)
      nodes.at(i)->offset += move;
}

// Moves a specified node to the front of the given vector
// until it hits a position (node) with the specified offset.
// Requires the vector to be sorted by ascending offset.
void Cladogram::moveTo(int offset, Node * node, vector<Node *> &v) {
  int i = (int)v.size() - 1;
  while(i > 0 && v.at(i) != node) --i;
  while(i > 0 && v.at(i-1)->offset > offset) {
    swap(v.at(i-1), v.at(i));
    --i;
  }
}

// Inject single roots into specified position i if possible,
// until we hit upTo.
void Cladogram::optimise_injectSingleRootAt(int pos, int upTo) {
  Node * r = roots.at(pos);
  Node * cand;
  for(int i = pos+1; i < upTo; ++i) {

    cand = roots.at(i);

    if(cand->size == 1 && fitsInto(r->offset, cand)) {
      moveOffsetsHigherThan(cand->offset, -1);
      cand->offset = r->offset;
      moveTo(r->offset, cand, nodes);  // update order of nodes in vector
      moveTo(r->offset, cand, roots);
      break;  // break because we try once again for every injected node anyway
    }

  }

}

// Fill gaps within tree lines, interleave subtrees (only by 1 offset)
void Cladogram::optimise_nextTree(int first, int last) {
  Node * a;
  Node * b = nodes.at(first);
  for(int i = first; i < last - 1; ++i) {

    a = b;
    b = nodes.at(i + 1);
    if(fitsInto(a->offset, b))
      moveOffsetsHigherThan(b->offset - 1, -1);

  }
}

// Pull nodes as near as possible to their parents.
void Cladogram::optimise_pullTree(int first, int last) {
  Node * n;
  int sign = 0;
  for(int i = first; i < last; ++i) {
    n = nodes.at(i);
    if(n->parent == NULL) continue;
    if(n->offset < n->parent->offset) sign = 1;
    else sign = -1;
    while(fitsInto(n->offset + sign, n)) n->offset += sign;
  }
}
