/*
*  gnuclad-portability.h - portability header
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

#ifndef GNUCLADPORTABILITY_H_
#define GNUCLADPORTABILITY_H_


#if defined (WIN32) || defined (_WIN32)
#define GNUCLAD_WINDOWS
#else
#define GNUCLAD_POSIX
#endif


#include <string>
#include <dirent.h>

extern bool islink(dirent * dirElem);

extern std::string folder_delimiter;





#endif
