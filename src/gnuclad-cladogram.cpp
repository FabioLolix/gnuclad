/*
*  gnuclad-cladogram.cpp - implements the Cladogram class for gnuclad
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

#include "gnuclad.h"

#include <iostream>
#include <algorithm>
#include <cmath>

using namespace std;

Cladogram::Cladogram() {

  maximumOffset = 0;
  inputFolder = "";
  truncateFolder = false;
  inVitro = false;

  endOfTime = currentDate();
  beginningOfTime = endOfTime;


  // the following default settings can be overwritten in the config file:

  infoBoxTitle = "Title";
  infoBoxTitleSize = 18;
  //~ infoBoxText.push_back("Lorem ipsum dolor sit amet");
  infoBoxTextSize = 12;
  infoBoxFont = "Liberation Sans, Arial, Helvetica";
  infoBoxFontColor = Color("#000");
  infoBoxColor1 = Color("#51b1f1");
  infoBoxColor2 = Color("#3181f1");
  infoBoxX = 10;
  infoBoxY = 45;
  infoBoxWidth = 166;
  infoBoxHeight = 60;

  slice = "";

  fontCorrectionFactor = 1.0;
  orientation = 0;
  treeMode = 0;
  sortKey = 0;
  optimise = 99;
  strictOverlaps = 0;
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

  labelFont = "Liberation Sans, Arial, Helvetica";
  labelFontSize = 16;
  labelFontColor = Color("#000");
  labelBGOpacity = 0;
  asciiStrings = 0;
  nameChangeType = 0;

  derivType = 0;
  dotRadius = 10;
  smallDotRadius = 5;
  dotType = 0;
  bigParent = 0;

  connectorDots = 1;
  connectorsDashed = 1;

  yearLinePX = 40;
  yearLineColor1 = Color("#037");
  yearLineColor2 = Color("#37d");
  yearLineFont = "Liberation Sans, Arial, Helvetica";
  yearLineFontSize = 28;
  yearLineFontColor = Color("#fff");
  appendYears = 0;

  daysInMonth = 30;
  monthsInYear = 12;

  descriptionType = 0;

  dir_showDotFiles = 0;
  dir_colorFile = Color("#0ff");
  dir_colorDir = Color("#00f");
  dir_colorLink = Color("#0f0");

  debug = 0;

}

Cladogram::~Cladogram() {
  for(int i = 0; i < (int)nodes.size(); ++i)
    if(nodes[i] != NULL) delete nodes[i];
  for(int i = 0; i < (int)connectors.size(); ++i)
    if(connectors[i] != NULL) delete connectors[i];
  for(int i = 0; i < (int)domains.size(); ++i)
    if(domains[i] != NULL) delete domains[i];

  for(int i = 0; i < (int)includeSVG.size(); ++i)
    if(includeSVG[i] != NULL) delete includeSVG[i];
  for(int i = 0; i < (int)includePNG.size(); ++i)
    if(includePNG[i] != NULL) delete includePNG[i];
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

    if(line == "" || line[0] == '#')
      continue;

    p = line.find("=");
    opt = line.substr(0, p);
    val = line.substr(p + 1, line.size());

    // strip trailing and leading whitespaces, and then quotes
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

    // assign options
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
      else if(opt == "slice") slice = val;
      else if(opt == "fontCorrectionFactor")fontCorrectionFactor=str2double(val);
      else if(opt == "orientation") orientation = str2int(val);
      else if(opt == "treeMode") treeMode = str2int(val);
      else if(opt == "sortKey") sortKey = str2int(val);
      else if(opt == "optimise") optimise = str2int(val);
      else if(opt == "strictOverlaps") strictOverlaps = str2int(val);
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
      else if(opt == "labelBGOpacity") labelBGOpacity = str2int(val);
      else if(opt == "asciiStrings") asciiStrings = str2int(val);
      else if(opt == "nameChangeType") nameChangeType = str2int(val);
      else if(opt == "derivType") derivType = str2int(val);
      else if(opt == "dotRadius") dotRadius = str2int(val);
      else if(opt == "smallDotRadius") smallDotRadius = str2int(val);
      else if(opt == "dotType") dotType = str2int(val);
      else if(opt == "bigParent") bigParent = str2double(val);
      else if(opt == "connectorDots") connectorDots = str2int(val);
      else if(opt == "connectorsDashed") connectorsDashed = str2int(val);
      else if(opt == "yearLinePX") yearLinePX = str2int(val);
      else if(opt == "yearLineColor1") yearLineColor1 = Color(val);
      else if(opt == "yearLineColor2") yearLineColor2 = Color(val);
      else if(opt == "yearLineFont") yearLineFont = val;
      else if(opt == "yearLineFontSize") yearLineFontSize = str2int(val);
      else if(opt == "yearLineFontColor") yearLineFontColor = Color(val);
      else if(opt == "appendYears") appendYears = str2int(val);
      else if(opt == "daysInMonth") daysInMonth = str2int(val);
      else if(opt == "monthsInYear") monthsInYear = str2int(val);
      else if(opt == "endOfTime") endOfTime = Date(val);
      else if(opt == "descriptionType") descriptionType = str2int(val);
      else if(opt == "dir_showDotFiles") dir_showDotFiles = str2int(val);
      else if(opt == "dir_colorFile") dir_colorFile = Color(val);
      else if(opt == "dir_colorDir") dir_colorDir = Color(val);
      else if(opt == "dir_colorLink") dir_colorLink = Color(val);
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


  // Prepend input folder to image files in order enable input from everywhere
  for(int i = 0; i < (int)nodes.size(); ++i)
    nodes[i]->iconfile = inputFolder + nodes[i]->iconfile;
  for(int i = 0; i < (int)includeSVG.size(); ++i)
    includeSVG[i]->filename = inputFolder + includeSVG[i]->filename;
  for(int i = 0; i < (int)includePNG.size(); ++i)
    includePNG[i]->filename = inputFolder + includePNG[i]->filename;

  // Basics
  // Juggle dates, warn for duplicates and assing parent pointers
  for(int i = 0; i < nCount; ++i) {

    n = nodes[i];

    // If "start" is newer than endOfTime, ignore that node
    if(endOfTime < n->start) {
      cout << "\nIgnoring " << n->name
           << " : starts after specified End Of Time";

      nodes.erase(nodes.begin() + i);
      --nCount;
      --i;
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

    // Naming node same as parent is not allowed
    string parName = n->parentName;
    if(n->name == parName)
      throw n->name + " has the same name as it's parent";

    // Find pointer to parent and check for duplicates
    for(int j = 0; j < nCount; ++j) {

      if(j > i && n->name == nodes[j]->name)  // might result in bad children
        cout << "\nWarning: " << n->name
             << " (entry " << j+1 << ") is already listed at position " << i+1;

      if(parName == nodes[j]->name)
        n->parent = nodes[j];

    }
    if(parName != "" && n->parent == NULL)
      throw "unable to find parent (" + parName +") for " + n->name;

    // Throw if node starts before parent and add node to parent's children
    if(n->parent != NULL) {

      if(n->start < n->parent->start)
        throw n->name + " starts before it's parent " + n->parent->name;

      if(n->parent->stop < n->start && inVitro == false)
        cout << "\nWarning: " << n->name
             << " starts AFTER it's parent (" << n->parent->name << ") stops!";

      n->parent->children.push_back(n);

    }
  }

  // Truncate folder names, used in dir parser
  if(truncateFolder == true)
    for(int i = 0; i < nCount; ++i) {
      n = nodes[i];
      n->name = n->name.substr(n->name.rfind(folder_delimiter) + 1);
    }

  // If node is not within slice, erase it
  if(slice != "") {
    Node * sliceNode = 0;
    for(int i = 0; i < nCount; ++i)
      if(nodes[i]->name == slice) sliceNode = nodes[i];
    if(sliceNode == NULL) throw "unable to slice node " + slice;

    sliceNode->parent = NULL;
    sliceNode->parentName = "";
    beginningOfTime = sliceNode->start;

    for(int i = 0; i < nCount; ++i) {
      n = nodes[i];
      if( n->name != slice && !n->derivesFrom(sliceNode) ) {
        nodes.erase(nodes.begin() + i);
        --nCount;
        --i;
      }
    }
  }

  // Assign nodes to domains
  for(int i = 0; i < dCount; ++i) {
    d = domains[i];
    for(int j = 0; j < nCount; ++j)
      if(nodes[j]->name == d->nodeName)
        d->node = nodes[j];

    if(d->node == NULL || endOfTime < d->node->start) {
      cout << "\nWarning: unable to assign domain to " + d->nodeName;
      domains.erase(domains.begin() + i);
      --dCount;
      --i;
    }
  }

  // Assign nodes to connectors
  for(int i = 0; i < cCount; ++i) {

    c = connectors[i];

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
      if(nodes[j]->name == c->fromName)
        c->from = nodes[j];
      if(nodes[j]->name == c->toName)
        c->to = nodes[j];
    }

    if(c->from == NULL || c->to == NULL) {
      cout << "\nWarning: unable to assign connector "
           << c->fromName << " -> " << c->toName;
      connectors.erase(connectors.begin() + i);
      --cCount;
      --i;
      continue;
    }
  }

  // Push through size
  // Requires full parent paths, hence a new pass
  for(int i = 0; i < nCount; ++i) {
    n = nodes[i];
    while(n->parent != NULL) {
      n->parent->size += 1;
      n = n->parent;
    }
  }

  // Build the map
  // Set offsets to all nodes
  // Requires correct size on all nodes, hence a new pass
  roots.clear();
  for(int i = 0; i < nCount; ++i)
    if(nodes[i]->parentName == "") roots.push_back( nodes[i]);
  int rCount = (int)roots.size();
  int offset = 0;
  for(int i = 0; i < rCount; ++i) {

    int treeOffset = 0;
    r = roots[i];
    std::deque<Node *> nodeTree;
    nodeTree.push_back(r);
    // Expand the tree if the current root size is bigger than 1
    while((int)nodeTree.size() < r->size) {

      n = nodeTree[treeOffset];
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
    // Loop will only pass once if it's a single root
    for(int j = 0; j < (int)nodeTree.size(); ++j)
      nodeTree[j]->offset = offset + j;

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

    // KEEP USAGE OF THE OPTIMISE VARIABLE WITHIN THIS LOOP

    if(optimise == 0) break;
    r = roots[i];

    if(r->size == 1) {  // Single root

      int opt = optimise/10;

      // Don't reach behind trees of certain size, depending on opt
      int treeBarrierSize[] = { 1, 1, 2, 2 ,2, 5, 10, 20, 50, 999999};  // 0 - 9
      if(opt > 9) opt = 9;
      int upTo = i+1;
      while(++upTo < rCount)
        if(roots[upTo]->size >= treeBarrierSize[opt]) break;
      if(upTo > rCount) upTo = rCount;

      optimise_injectSingleRootAt(i, upTo);

    } else {  // Tree root

      int opt = optimise%10;

      // Get first and last node positions of current continuous root tree block
      int first = 0;
      while(first < nCount && nodes[first]->root() != r) ++first;
      int last = first;
      while(last < nCount && nodes[last]->root() == r) ++last;

      if(opt >= 1) optimise_nextTree(first, last);

      //~ if(opt >= 6) {                                                        <= pseudocode
        //~ optimise_interleaveTree(first, last);
        //~ if(opt >= 8)
          //~ optimise_interleaveAllRoots(first, last);
      //~ }

      if(opt >= 6)
        optimise_pullToRoot(first, last, (opt>=8)?(true):(false) );
      if(opt >= 3 && opt < 8)
        optimise_pullToParent(r, first, last);

    }
  }

  // Insert spacing at tree edges
  for(int i = 0; i < nCount - 1; ++i) {
    Node * a = nodes[i];
    Node * b = nodes[i+1];
    if( a->root() != b->root() )  // if not in same tree
      if(a->parent != NULL || b->parent != NULL ||  // if not root nodes
         a->children.size() > 0 || b->children.size() > 0)  // or have children
        if(a->root()->size > treeSpacingBiggerThan ||  // honor option
           b->root()->size > treeSpacingBiggerThan)
          moveOffsetsHigherThan(a->offset, treeSpacing);
  }

  // Insert spacing at fat lines (bigParent) 
  // if the overlap goes more than 60% of the way.
  if(bigParent > 0) {
    for(int i = 0; i < nCount - 1; ++i) {
      n = nodes[i];
      if( (lineWidth * (sqrt(n->size)-1) * bigParent) / offsetPX  >  0.6 ) {
        moveOffsetsHigherThan(n->offset-1, 1);
        moveOffsetsHigherThan(n->offset, 1);
      }

    }
  }

  // Set domain offsets
  for(int i = 0; i < dCount; ++i) {
    d = domains[i];

    if(d->node->size < treeSpacingBiggerThan)
      cout << "\nWARNING: domain of node " << d->node->name << " will not get"
           << " spaced and will possibly overlap with others because it's tree"
           << " is too small. Reduce the treeSpacingBiggerThan config option.";

    // Get minimum and maximum offsets of children this domain's node has
    int min = nCount-1, max = 0;
    for(int j = 0; j < nCount; ++j) {
      n = nodes[j];
      if(n->derivesFrom(d->node)) {
        if(n->offset < nodes[min]->offset)
          min = j;
        if(n->offset > nodes[max]->offset)
          max = j;
      }
    }

    d->offsetA = nodes[min]->offset;
    d->offsetB = nodes[max]->offset;
    if(d->offsetA > d->node->offset) d->offsetA = d->node->offset;
    if(d->offsetB < d->node->offset) d->offsetB = d->node->offset;
  }

  // Set connector offsets
  for(int i = 0; i < cCount; ++i) {
    c = connectors[i];
    c->offsetA = c->from->offset;
    c->offsetB = c->to->offset;
    //~ if(c->offsetA > c->offsetB)   // this looks bad with dashed connectors
      //~ swap(c->offsetA, c->offsetB);
  }

  // Get total size (the maximum offset)
  for(int i = 0; i < nCount; ++i)
    if(maximumOffset < nodes[i]->offset)
      maximumOffset = nodes[i]->offset;


  if(debug > 0) debug_cladogram_compute();

}


// Prints some debug info
void Cladogram::debug_cladogram_compute() {

  for(int i = 0; i < (int)nodes.size(); ++i) {
    Node * n = nodes[i];
    string rt = (n->parent == NULL)?(" *"):("");
    cout << "\nDEBUG\t" << n->name << rt
         << "    \t  \tsize: " << n->size << "    \toffset: " << n->offset;
  }
  cout << "\n";
  for(int i = 0; i < (int)connectors.size(); ++i) {
    Connector * c = connectors[i];
    cout << "\nDEBUG\t" << c->fromName << "  \t=>\t" << c->toName
         << "   \t(" << c->offsetA << ","<< c->offsetB << ")";
  }
  cout << "\n";
  for(int i = 0; i < (int)domains.size(); ++i) {
    Domain * d = domains[i];
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
                                                                                //TODO: OPTIMISE HERE??
  // optimise: if node i (has no children and) ends before i+1 starts, put them on same side


  // Insert the upper subtree before current node
  for(int i = 1; i < childCount; i += 2)
    tree.insert(tree.begin() + pos + i/2, n->children[i]);

  // Insert the lower subtree after current node in reverse order
  for(int i = 0; i < childCount; i += 2)
    tree.insert(tree.begin() + pos + (childCount/2) + 1, n->children[i]);

}

// Generates a treeMap with the children only below the parent
// Insert all child nodes after current node in reverse order
void Cladogram::compute_subtreeLower(std::deque<Node *> &tree,
                                    int pos, Node * n) {
  for(int i = 0; i < (int)n->children.size(); ++i)
    tree.insert(tree.begin() + pos + 1, n->children[i]);
}

// Generates a treeMap with the children only below the parent - inverse order
// Insert all child nodes after current node
void Cladogram::compute_subtreeLowerInverse(std::deque<Node *> &tree,
                                    int pos, Node * n) {
  for(int i = 0; i < (int)n->children.size(); ++i)
    tree.insert(tree.begin() + pos + 1 + i, n->children[i]);
}

// Returns true if specified node fits into given offset
// without overlapping other nodes in that offset.
// False otherwise.
bool Cladogram::fitsInto(const int offset, Node * node) {

  vector<Node *> tmp;
  for(int i = 0; i < (int)nodes.size(); ++i)
    if(nodes[i]->offset == offset)
      tmp.push_back(nodes[i]);

  if(offset == node->offset) return false;
  if(tmp.size() == 0) return true;

  sort(tmp.begin(), tmp.end(), compareDate());

  int DinM = daysInMonth;
  int MinY = monthsInYear;

  // Compare node to first and last in tmp, then to all others
  if( rOf(node->stop + stopSpacing, MinY, DinM) < tmp[0]->start ||
      rOf(tmp[tmp.size()-1]->stop + stopSpacing, MinY, DinM) < node->start )
    return true;
  for(int i = 0; i < (int)tmp.size() - 2; ++i)
    if( rOf(tmp[i]->stop + stopSpacing, MinY, DinM) < node->start &&
        rOf(node->stop + stopSpacing, MinY, DinM) < tmp[i+1]->start )
      return true;

  return false;
}

// Move all nodes with an offset higher than the specified one by given value.
void Cladogram::moveOffsetsHigherThan(const int offset, const int move) {
  for(int i = 0; i < (int)nodes.size(); ++i)
    if(nodes[i]->offset > offset)
      nodes[i]->offset += move;
}

// Moves a specified node to the front of the given vector
// until it hits a position (node) with the specified offset.
// Requires the vector to be sorted by ascending offset.
void Cladogram::moveTo(int offset, Node * node, vector<Node *> &v) {
  int i = (int)v.size() - 1;
  while(i > 0 && v[i] != node) --i;
  while(i > 0 && v[i-1]->offset > offset) {
    swap(v[i-1], v[i]);
    --i;
  }
}

// Inject single roots into specified position i if possible,
// until we hit upTo.
void Cladogram::optimise_injectSingleRootAt(int pos, int upTo) {
  Node * r = roots[pos];
  Node * cand;

  for(int i = pos+1; i < upTo; ++i) {

    cand = roots[i];

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
  Node * b = nodes[first];
  for(int i = first; i < last - 1; ++i) {

    a = b;
    b = nodes[i + 1];
    if(fitsInto(a->offset, b))
      moveOffsetsHigherThan(b->offset - 1, -1);

  }
}

// Pull nodes to towards the root.
void Cladogram::optimise_pullToRoot(int first, int last, bool stronger) {
  Node * n;
  int sign = 0;

  // Sort by distance to root
  stable_sort(nodes.begin()+first, nodes.begin()+last, compareRootDist());

  int rootoffset = nodes[first]->root()->offset;
  for(int i = first; i < last; ++i) {
    n = nodes[i];
    if(n->parent == NULL) continue;
    if(n->offset < rootoffset) sign = 1;
    else sign = -1;

    int oldOffset = n->offset;

    while(fitsInto(n->offset + sign, n)) {
      if(n->offset == rootoffset) break;
      if(n->offset < 0)break;
      if(n->offset > nodes[last-1]->offset) break;
      if(stronger == false && n->offset == n->parent->offset) break;

      bool overlaps = optimise_strictOverlaps(n, oldOffset, sign, first, last);
      if(overlaps == false) n->offset += sign;  // pull by one offset
      else break;
    }

    // Remove empty offsets
    Node dummy;
    dummy.offset = -1;
    dummy.start = beginningOfTime;
    dummy.stop = endOfTime;
    if(fitsInto(oldOffset, &dummy)) moveOffsetsHigherThan(oldOffset, -1);
  }

  // Get nodes back into offset order
  stable_sort(nodes.begin()+first, nodes.begin()+last, compareOffset());
}

// Pull nodes to their parents.
void Cladogram::optimise_pullToParent(Node * r, int first, int last) {

  if(r->size == 1) return;

  Node * n;
  int sign = 0;

  // Sort by distance to parent
  stable_sort(r->children.begin(), r->children.end(), compareParDist());

  for(int i = 0; i < (int)r->children.size(); ++i) {
    n = r->children[i];
    if(n->offset < n->parent->offset) sign = 1;
    else sign = -1;
    int oldOffset = n->offset;

    while(fitsInto(n->offset + sign, n)) {
      if(n->offset < 0)break;
      if(sign == -1 && n->offset <= r->offset - r->size) break;
      if(sign ==  1 && n->offset >= r->offset + r->size) break;

      bool overlaps = optimise_strictOverlaps(n, oldOffset, sign, first, last);
      if(overlaps == false) n->offset += sign;
      else break;
    }

    // Remove empty offsets
    Node dummy;
    dummy.offset = -1;
    dummy.start = beginningOfTime;
    dummy.stop = endOfTime;
    if(fitsInto(oldOffset, &dummy)) moveOffsetsHigherThan(oldOffset, -1);

  }

  for(int i = 0; i < (int)r->children.size(); ++i)
    optimise_pullToParent(r->children[i], first, last);

  // Get children back into offset order
  stable_sort(r->children.begin(), r->children.end(), compareOffset());
}

// Aesthetical hack to prevent node lines overlapping deriv lines
bool Cladogram::optimise_strictOverlaps(Node * n, int oldOffset, int sign,
                                        int first, int last) {
  if(strictOverlaps == 0) return false;
  if(derivType < 1 || 4 < derivType) return false;

  double slope = double((n->offset - n->parent->offset)*offsetPX) * -sign
                 / double(datePX(n->stop + stopSpacing, this)
                        - datePX(n->parent->start, this));
  int addPX = offsetPX/slope * (n->offset - oldOffset        +1);
  int stopPX = datePX(n->stop, this) + addPX;
  int startPX = datePX(n->start, this);

  /////////////////////////
  // The following is an additional implementation of fitsInto(), based
  // on PX rather than Date. When moving into a function, use and modify
  // fitsInto() instead of this one (many checks removed)!
  vector<Node *> tmp;
  for(int i = first; i < last; ++i)
    if(nodes[i]->offset == n->offset)
      tmp.push_back(nodes[i]);

  if(tmp.size() == 0) return false;

  sort(tmp.begin(), tmp.end(), compareDate());

  if( stopPX < datePX(tmp[0]->start, this) ||
      startPX > datePX(n->stop + stopSpacing, this) )
    return false;
  for(int i = 0; i < (int)tmp.size() - 2; ++i)
    if( datePX(tmp[i]->stop + stopSpacing, this) < startPX &&
        stopPX < datePX(tmp[i+1]->start, this))
      return false;
  /////////////////////////
  return true;
}


// Change node array sequence to "pseudo-inverse" preorder
// Fix for SVG layering (derivType 2 - 4)
void Cladogram::nodesPreorder() {
  int dbg_counter = 0, dbg_swaps = 0;
  for(int i = (int)nodes.size() - 1; i >= 0; --i) {
    Node * par = nodes[i]->parent;
    if( par == NULL) continue;

    int j = (int)nodes.size();
    while(--j >= 0) {  // find parent
      dbg_counter++;
      if(nodes[j] == par) break;
    }
    if(j < i) {  // swap with parent if parent has lower position
      dbg_swaps++;
      swap(nodes[j], nodes[i]);
      ++i;
    }

  }
  if(debug > 0)
    cout << "\nnodesPreorder: comparisons=" << dbg_counter
         << "  swaps=" << dbg_swaps;
}
