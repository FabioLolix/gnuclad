/*
*  gnuclad.h - header for gnuclad
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


#ifndef GNUCLAD_H_
#define GNUCLAD_H_



//~ #include <string>
//~ #include <exception>
#include <vector>
#include <fstream>
#include <sstream>
#include <deque>

#include "gnuclad-portability.h"



////////////////////////////////////////////////////////////////////////////////
///
// Basic Classes
//

class Color {
  public:
  int red;
  int green;
  int blue;
  std::string hex;

  Color();
  Color(int tred, int tgreen, int tblue);
  Color(std::string thex);
};

class Date {
  public:
  int year;
  int month;
  int day;

  bool monthset;
  bool dayset;

  Date();
  Date(int tyear, int tmonth, int tday);
  Date(int tyear, int tmonth);
  Date(int tyear);
  Date(const std::string str);
  bool operator<(const Date d);
  bool operator==(Date d);
  Date operator+(const Date d);
};

class Image {
  public:
  std::string filename;
  int x;
  int y;

  Image();
  Image(std::string tfilename);
};

class NameChange {
  public:
  std::string newName;
  Date date;
  std::string description;

  NameChange();
  NameChange(std::string tnewName, Date tdate, std::string description);
};

class Node {
  public:
  std::string name;
  std::string parentName;
  std::vector<NameChange> nameChanges;
  Date start;
  Date stop;
  Color color;
  std::string iconfile;
  std::string description;

  Node * parent;
  std::vector<Node *> children;

  int size;
  int offset;

  Node();
  void addNameChange(std::string newName, Date date, std::string description);
  Node * root();
  bool derivesFrom(Node * p);
};

class Domain {
  public:
  std::string nodeName;
  Color color;
  int intensity;

  Node * node;
  int offsetA;
  int offsetB;

  Domain();
};

class Connector {
  public:
  std::string fromName;
  std::string toName;
  Date fromWhen;
  Date toWhen;
  int thickness;
  Color color;

  Node * from;
  Node * to;

  int offsetA;
  int offsetB;

  Connector();
};

class InputFile {
  public:
  std::ifstream * p;
  std::string name;

  InputFile(std::string tname);
  ~InputFile();
};

class OutputFile {
  public:
  std::ofstream * p;
  std::string name;

  OutputFile(std::string tname);
  ~OutputFile();
};



////////////////////////////////////////////////////////////////////////////////
///
// Cladogram Class
//

class Cladogram {

  private:
  void debug_cladogram_compute();

  void compute_subtreeBoth(std::deque<Node *> &tree, int pos, Node * n);
  void compute_subtreeLower(std::deque<Node *> &tree, int pos, Node * n);
  void compute_subtreeLowerInverse(std::deque<Node *> &tree, int pos, Node * n);

  bool fitsInto(const int offset, Node * node);
  void moveOffsetsHigherThan(const int offset, const int move);
  void moveTo(int offset, Node * node, std::vector<Node *> &v);

  void optimise_injectSingleRootAt(int i, int upTo);
  void optimise_nextTree(int first, int last);
  void optimise_pullTree(int first, int last);

  public:
  std::vector<Node *> nodes;
  std::vector<Node *> roots;
  std::vector<Connector *> connectors;
  std::vector<Domain *> domains;
  std::vector<Image *> includeSVG;
  std::vector<Image *> includePNG;

  std::string inputFolder;
  std::string gnuclad_version;


  // CONFIG OPTIONS BEGIN

  int maximumOffset;
  Date beginningOfTime;

  // The options below can be overridden by the config file

  Date endOfTime;

  std::string infoBoxTitle;
  std::vector<std::string> infoBoxText;
  int infoBoxTitleSize;
  std::string infoBoxFont;
  int infoBoxTextSize;
  Color infoBoxFontColor;
  Color infoBoxColor1;
  Color infoBoxColor2;
  int infoBoxX;
  int infoBoxY;
  int infoBoxWidth;
  int infoBoxHeight;

  double fontCorrectionFactor;
  int orientation;
  int treeMode;
  int sortKey;
  int optimise;
  int treeSpacing;
  int treeSpacingBiggerThan;

  Color mainBackground;
  int rulerWidth;
  Color rulerColor;
  int rulerMonthWidth;
  Color rulerMonthColor;

  int lineWidth;
  int offsetPX;
  int stopFadeOutPX;
  Date stopSpacing;
  int yearPX;

  std::string labelFont;
  int labelFontSize;
  Color labelFontColor;
  int labelBGOpacity;
  int nameChangeType;

  int derivType;
  int dotRadius;
  int smallDotRadius;
  int dotType;

  int connectorDots;
  int connectorsDashed;

  int yearLinePX;
  Color yearLineColor1;
  Color yearLineColor2;
  std::string yearLineFont;
  int yearLineFontSize;
  Color yearLineFontColor;

  int monthsInYear;
  int daysInMonth;
  int descriptionType;

  int debug;

  // CONFIG OPTIONS END


  Cladogram();
  ~Cladogram();

  void parseOptions(const std::string filename);
  void compute();

  Node * addNode(std::string tname);
  Domain * addDomain(std::string tname);
  Connector * addConnector();
  Image * addImage(std::string tname, std::vector<Image *> &vector);

};



////////////////////////////////////////////////////////////////////////////////
///
// Parser and Generator
//

class Parser {
  public:
  virtual ~Parser();
  virtual void parseData(Cladogram * cladogram, InputFile & in) = 0;
};

class Generator {
  public:
  virtual ~Generator();
  virtual void writeData(Cladogram * cladogram, OutputFile & out) = 0;
};



////////////////////////////////////////////////////////////////////////////////
///
// Helper Function Definitions
//

void pressEnter();
std::string strToLower(std::string str);
std::string getBaseFolder(std::string fname);
std::string getBaseName(std::string fname);
std::string getExt(std::string fname);
std::ifstream * new_infile(const std::string fname);
std::ofstream * new_outfile(const std::string fname);
void safeClose(std::ifstream * fp);
void safeClose(std::ofstream * fp);
void explode(const std::string str, const char delimiter,
             std::vector<std::string> * v);
void explodeSafely(const std::string str, const char delimiter,
                   const char toggle, std::vector<std::string> * v);
std::string findReplace(std::string str, std::string find, std::string replace);
double str2double(const std::string str);
int str2int(const std::string s);
std::string int2str(const int n);
std::string base64_encode(const char * raw, unsigned int len);
Date currentDate();
std::string Date2str(Date date);
int datePX(Date d, const Cladogram * clad);
Date rOf(Date d, int monthsInYear, int daysInMonth);
std::string checkHexCol(const std::string color);
std::string rgb2hexHue(int hue);
int hex2rgbHue(std::string hhue);



////////////////////////////////////////////////////////////////////////////////
///
// Sort Comparison Functors
//

struct compareDate : public std::binary_function<Node *,Node *,bool> {
  // Returns true if n1 starts before n2, otherwise false

	inline bool operator()(const Node * n1, const Node * n2) {
		if(n1->start.year < n2->start.year)
      return true;
    if(n1->start.year == n2->start.year && n1->start.month < n2->start.month)
      return true;
    if(n1->start.year == n2->start.year && n1->start.month == n2->start.month &&
       n1->start.day < n2->start.day)
      return true;
    return false;
	}
};
struct compareOffset : public std::binary_function<Node *,Node *,bool> {
  // Returns true if n1 has a lower offset than n2, otherwise false

	inline bool operator()(const Node * n1, const Node * n2) {
		if(n1->offset < n2->offset)
      return true;
    return false;
	}
};
struct compareName : public std::binary_function<Node *,Node *,bool> {
  // Returns true if n1 has a lower lexicographic name than n2, otherwise false

	inline bool operator()(const Node * n1, const Node * n2) {
		if(n1->name < n2->name)
      return true;
    return false;
	}
};



#endif
