/*
*  gnuclad-portability.h - a few definitions to help gnuclad be more portable
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


#if defined (WIN32) || defined (_WIN32)
#define GNUCLAD_WINDOWS
#else
#define GNUCLAD_POSIX
#endif



#ifdef GNUCLAD_POSIX

std::string folder_delimiter = "/";

#elif defined(GNUCLAD_WINDOWS)

std::string folder_delimiter = "\\";

#endif
