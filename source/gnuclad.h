/*
*  gnuclad.h - header for gnuclad
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


#ifndef GNUCLAD_H_
#define GNUCLAD_H_



//~ #include <string>
//~ #include <exception>
#include <vector>
#include <fstream>
#include <sstream>



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

  Date();
  Date(int tyear, int tmonth);
  Date(int tyear, int tmonth, int tday);
  Date(const std::string str);
  bool operator<(Date d);
  Date operator+(Date d);
};

class NameChange {
  public:
  std::string newName;
  Date date;

  NameChange();
  NameChange(std::string tnewName, Date tdate);
};

class Icon {
  public:
  std::string filename;

  Icon();
  Icon(std::string tfilename);
};

class Node {
  public:
  std::string name;
  std::string parentName;
  std::vector<NameChange> nameChanges;
  Date start;
  Date stop;
  Color color;
  Icon icon;
  std::string description;

  Node * parent;
  std::vector<Node *> children;

  int size;
  int offset;

  Node();
  void addNameChange(std::string newName, Date date);
  Node * root();
  bool derivesFrom(Node * p);
};

class Domain {
  public:
  std::string nodeName;
  Color color;
  int intensity;  // %

  Node * node;
  int offsetA;
  int offsetB;

  Domain();
};

class Connector {
  public:
  std::string fromName;
  std::string toName;
  Date when;
  int thickness;
  Color color;

  Node * from;
  Node * to;

  int offsetA;
  int offsetB;

  Connector();
};



////////////////////////////////////////////////////////////////////////////////
///
// Cladogram Class
//

class Cladogram {

  private:
  void debug_cladogram_compute();
  bool fitsInto(const int offset, Node * node);
  void moveOffsetsHigherThan(const int offset, const int move);
  void moveTo(int offset, Node * node, std::vector<Node *> &v);

  void optimise_injectSingleRootAt(int i, int upTo);
  void optimise_nextTree(int first, int last);

  public:
  std::vector<Node *> nodes;
  std::vector<Node *> roots;
  std::vector<Connector *>connectors;
  std::vector<Domain *>domains;

  Date beginningOfTime;
  Date endOfTime;
  int maximumOffset;


  std::string infoBoxTitle;
  std::string infoBoxText;
  int infoBoxTitleSize;
  std::string infoBoxFont;
  int infoBoxTextSize;
  Color infoBoxColor1;
  Color infoBoxColor2;
  int infoBoxX;
  int infoBoxY;
  int infoBoxWidth;
  int infoBoxHeight;

  int sortKey;
  int optimise;
  int treeSpacing;

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
  int labelBackground;

  int derivType;
  int dotRadius;
  int smallDotRadius;

  int connectorsDashed;

  int yearLinePX;
  Color yearLineColor1;
  Color yearLineColor2;
  std::string yearLineFont;
  int yearLineFontSize;
  Color yearLineFontColor;

  int monthsInYear;
  int daysInMonth;
  int descriptionIsHyperLink;

  int debug;


  Cladogram();
  ~Cladogram();

  void parseOptions(const std::string filename);
  void compute();

  Node * addNode(std::string tname);
  Node * addNode(std::string tname, Color tcolor,
                 Date tstart, Date tstop, std::string tparentName);
  Domain * addDomain(std::string tname);
  Connector * addConnector();

  Date rOf(Date d);
};



////////////////////////////////////////////////////////////////////////////////
///
// Parser and Generator
//

class Parser {
  public:
  virtual void parseData(Cladogram * cladogram, std::ifstream * fp) = 0;
};

class Generator {
  public:
  virtual void writeData(Cladogram * cladogram, std::ofstream * fp) = 0;
};



////////////////////////////////////////////////////////////////////////////////
///
// Helper Function Definitions
//

void pressEnter();
std::string strToLower(std::string str);
std::string getBaseName(std::string fname);
std::string getExt(std::string fname);
std::ifstream * new_infile(const std::string fname);
std::ofstream * new_outfile(const std::string fname);
void safeClose(std::ifstream * fp);
void safeClose(std::ofstream * fp);
void explode(const std::string str, const char delimiter, std::vector<std::string> * v);
void explodeSafely(const std::string str, const char delimiter,
                   const char toggle, std::vector<std::string> * v);
std::string findReplace(std::string str, std::string find, std::string replace);
int str2int(const std::string s);
std::string int2str(const int n);
Date currentDate();
int datePX(Date d, const Cladogram * clad);
std::string ckhexcol(const std::string color);



////////////////////////////////////////////////////////////////////////////////
///
// Sort Comparison Definitions
//

struct compareDate : public std::binary_function<Node *,Node *,bool> {
  // Returns true if n1 starts before n2, otherwise false

	inline bool operator()(const Node * n1, const Node * n2) {
		if(n1->start.year < n2->start.year)
      return true;
    if(n1->start.year == n2->start.year && n1->start.month < n2->start.month)
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
