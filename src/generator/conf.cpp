/*
*  conf.cpp - implements a configuration file generator for gnuclad
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

#include "conf.h"

using namespace std;


GeneratorCONF::GeneratorCONF() {}
GeneratorCONF::~GeneratorCONF() {}

void GeneratorCONF::writeData(Cladogram * clad, OutputFile & out) {

  ofstream & f = *(out.p);

  f << "# gnuclad config file\n\n"
    << "# This configuration file has been generated by gnuclad "
    << clad->gnuclad_version << "\n"
    << "\n"
    << "# If you comment or delete an option,\n"
    << "# gnuclad will use the built-in defaults.\n"
    << "# Allowed syntax is:\n"
    << "#   option=value\n"
    << "#   option = value\n"
    << "#   option= 'value'\n"
    << "#   option =\"value\"\n"
    << "#   option = three word value\n"
    << "#   option = \"three word value\"\n"
    << "#   ...\n\n";

  f << "\n# Modify the appearance of your info box"
    << "\n# To disable the box:"
    << "\n# Set both font size parameters to 0, set the width and height to 0"
    << "\ninfoBoxTitle = " << clad->infoBoxTitle
    << "\ninfoBoxTitleSize = " << clad->infoBoxTitleSize;

  if(clad->infoBoxText.size() == 0)
    f << "\ninfoBoxText = Lorem ipsum dolor sit amet,"
      << "\ninfoBoxText = consectetuer adipiscing elit";
  else for(unsigned int i = 0; i < clad->infoBoxText.size(); ++i)
    f << "\ninfoBoxText = " << clad->infoBoxText[i];

  f << "\ninfoBoxTextSize = " << clad->infoBoxTextSize
    << "\ninfoBoxFont = " << clad->infoBoxFont
    << "\ninfoBoxFontColor = #" << clad->infoBoxFontColor.hex
    << "\ninfoBoxColor1 = #" << clad->infoBoxColor1.hex
    << "\ninfoBoxColor2 = #" << clad->infoBoxColor2.hex
    << "\ninfoBoxX = " << clad->infoBoxX
    << "\ninfoBoxY = " << clad->infoBoxY
    << "\ninfoBoxWidth = " << clad->infoBoxWidth
    << "\ninfoBoxHeight = " << clad->infoBoxHeight
    << "\n"
    << "\n# Correction factor for font pixel width/height."
    << "\n# It is normalised for Liberation Sans (= Arial) and will"
    << "\n# affect the alignment of all text in your output. Since this"
    << "\n# one option affects all text instances, it's recommended"
    << "\n# to use 'similarly spaced' fonts throughout your settings."
    << "\nfontCorrectionFactor = " << clad->fontCorrectionFactor
    << "\n"
    << "\n# Orientation of the timeline"
    << "\n# 0 = left to right, 1 = top to bottom"
    << "\n# 2 = right to left, 3 = bottom to top"
    << "\norientation = " << clad->orientation
    << "\n"
    << "\n# The layout of the tree"
    << "\n# 0 = branch to both sides"
    << "\n# 1 = branch only on lower side"
    << "\n# 2 = branch only on lower side, inverse (good for CSV output)"
    << "\ntreeMode = " << clad->treeMode
    << "\n"
    << "\n# Chose: 0 = don't presort, 1 = by name, 2 = by date"
    << "\nsortKey = " << clad->sortKey
    << "\n"
    << "\n# Set optimisation between 0 and 99. See manual for details."
    << "\n# First digit: isolated nodes. Second digit: trees."
    << "\noptimise = " << clad->optimise
    << "\n"
    << "\n# How much space (in offsets) to add before/after trees"
    << "\ntreeSpacing = " << clad->treeSpacing
    << "\n"
    << "\n# How big (total children) a tree has to be before it gets spaced"
    << "\ntreeSpacingBiggerThan = " << clad->treeSpacingBiggerThan
    << "\n"
    << "\n# Background color in hexadecimal RGB (#abc or #abcdef)"
    << "\nmainBackground = #" << clad->mainBackground.hex
    << "\n"
    << "\n# Customise the background lines separating the years and months"
    << "\nrulerWidth = " << clad->rulerWidth
    << "\nrulerColor = #" << clad->rulerColor.hex
    << "\nrulerMonthWidth = " << clad->rulerMonthWidth
    << "\nrulerMonthColor = #" << clad->rulerMonthColor.hex
    << "\n"
    << "\n# Width of all node lines"
    << "\nlineWidth = " << clad->lineWidth
    << "\n"
    << "\n# Number of pixels between node lines (== 1 offset)"
    << "\noffsetPX = " << clad->offsetPX
    << "\n"
    << "\n# Set to 0 if you want 'dying' lines to stop abruptly"
    << "\nstopFadeOutPX = " << clad->stopFadeOutPX
    << "\n"
    << "\n# When optimising, gnuclad will inline nodes if possible. This sets"
    << "\n# the minimum time distance between the stop of one node and the"
    << "\n# start of another which will allow the latter to get appended."
    << "\n# Format: 'y.m.d' or 'y.m' or 'y' (year, month, day)"
    << "\nstopSpacing = " << Date2str(clad->stopSpacing)
    << "\n"
    << "\n# How many pixels one year should have"
    << "\nyearPX = " << clad->yearPX
    << "\n"
    << "\n# Customise the node labels"
    << "\nlabelFont = " << clad->labelFont
    << "\nlabelFontSize = " << clad->labelFontSize
    << "\nlabelFontColor = #" << clad->labelFontColor.hex
    << "\n"
    << "\n# An experimental feature you might want to use with derivType = 1"
    << "\n# Opacity takes values between 0 (transparent) and 100 (opaque)."
    << "\n# It DOES NOT work flawlessly with SVG 1.1 output."
    << "\n# (hoping for v1.2 or CSS3 ...)"
    << "\nlabelBGOpacity = " << clad->labelBGOpacity
    << "\n"
    << "\n# This sets the heuristic used for determining label background width"
    << "\n# 0 = standard, will sometimes generate suboptimal width values"
    << "\n# 1 = improved, but only if the majority of your characters fall into"
    << "\n#               the ASCII charset and you use a fairly standard font"
    << "\n#               playing around with fontCorrectionFactor might help"
    << "\nasciiStrings = " << clad->asciiStrings
    << "\n"
    << "\n# Useful if you want to use the renames only as version bumps."
    << "\n# 0 = rename above the dot to the right, like the first name"
    << "\n# 1 = rename centered within the dot"
    << "\nnameChangeType = " << clad->nameChangeType
    << "\n"
    << "\n# Set the way the lines should derive from the parent."
    << "\n# 0 = orthogonal, 1 = from parent's starting point"
    << "\nderivType = " << clad->derivType
    << "\n"
    << "\n# The size of the node dots"
    << "\ndotRadius = " << clad->dotRadius
    << "\nsmallDotRadius = " << clad->smallDotRadius
    << "\n"
    << "\n# Determines how the dots look."
    << "\n# 0 = full dots, 1 = circles"
    << "\n# Circles might be useful together with nameChangeType = 1"
    << "\ndotType = " << clad->dotType
    << "\n"
    << "\n# Adds small half-circles to connector starting points"
    << "\nconnectorDots = " << clad->connectorDots
    << "\n"
    << "\n# 1 = connectors have a dashed stroke"
    << "\n# 0 = connectors are full lines"
    << "\nconnectorsDashed = " << clad->connectorsDashed
    << "\n"
    << "\n# Customise the year line encompassing the cladogram."
    << "\nyearLinePX = " << clad->yearLinePX
    << "\nyearLineColor1 = #" << clad->yearLineColor1.hex
    << "\nyearLineColor2 = #" << clad->yearLineColor2.hex
    << "\nyearLineFont = " << clad->yearLineFont
    << "\nyearLineFontSize = " << clad->yearLineFontSize
    << "\nyearLineFontColor = #" << clad->yearLineFontColor.hex
    << "\n"
    << "\n# Remember to adjust your input data after changing these averages."
    << "\ndaysInMonth = " << clad->daysInMonth
    << "\nmonthsInYear = " << clad->monthsInYear
    << "\n"
    << "\n# Not setting endOfTime will use the current date.";
  if(clad->endOfTime == currentDate())
    f << "\n#endOfTime = 2012.12";
  else
    f << "\nendOfTime = " << Date2str(clad->endOfTime);

  f << "\n"
    << "\n# How the description field should be used."
    << "\n# 0 = ignore"
    << "\n# 1 = weblink URL"
    << "\ndescriptionType = " << clad->descriptionType
    << "\n"
    << "\n# When parsing direcories, show dot (hidden) files (0 = off, 1 = on)"
    << "\ndir_showDotFiles = " << clad->dir_showDotFiles
    << "\n"
    << "\n# Show debug information when running gnuclad. (0 = off, 1 = on)"
    << "\ndebug = " << clad->debug
    << "\n";

}
