/* +---------------------------------------------------------------------------+
   |                          The Mooxygen project                             |
   |                    https://github.com/jlblancoc/mooxygen                  |
   |                                                                           |
   | Copyright (C) 2009-2010 Jose Luis Blanco <joseluisblancoc at gmail.com >  |
   |                                                                           |
   |    This software was written by Jose Luis Blanco,                         |
   |      University of Malaga (Spain).                                        |
   |                                                                           |
   |  This file is part of the Mooxygen project.                               |
   |                                                                           |
   |     Mooxygen is free software: you can redistribute it and/or modify      |
   |     it under the terms of the GNU General Public License as published by  |
   |     the Free Software Foundation, either version 3 of the License, or     |
   |     (at your option) any later version.                                   |
   |                                                                           |
   |   Mooxygen is distributed in the hope that it will be useful,             |
   |     but WITHOUT ANY WARRANTY; without even the implied warranty of        |
   |     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         |
   |     GNU General Public License for more details.                          |
   |                                                                           |
   |     You should have received a copy of the GNU General Public License     |
   |     along with Mooxygen.  If not, see <http://www.gnu.org/licenses/>.     |
   |                                                                           |
   +---------------------------------------------------------------------------+ */

#ifndef CMooxygenOptions_H
#define CMooxygenOptions_H

#include "moox_utils.h"

namespace mooxygen
{
	using namespace std;

	/// To store and parse the configuration file (the "Mooxyfile")
	class CMooxygenOptions
	{
		Str2StrMap	m_defaults;
		Str2StrMap			m_data;
	public:
		CMooxygenOptions();

		bool saveTemplateFile(const string &f) const; /// false on error

		bool loadFromFile(const string &f); /// false on error
		string get(const string &f) const; /// Empty if not found



	};
}
#endif

