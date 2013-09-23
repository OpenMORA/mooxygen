/* +---------------------------------------------------------------------------+
   |                          The Mooxygen project                             |
   |                    http://code.google.com/p/mooxygen/                     |
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

#include "CMooxygenOptions.h"

using namespace std;
using namespace mooxygen;

CMooxygenOptions::CMooxygenOptions()
{
	// Init default values:
	if (!m_defaults.empty()) return;

	m_defaults.clear();
	m_defaults["PROJECT"]="YOUR PROJECT NAME HERE";

	m_defaults["INPUT_PATH"]="./";
	m_defaults["MISSIONS_PATH"]="./";

	m_defaults["OUT_HTML"]="YES";
	m_defaults["OUT_HTML_PATH"]="./html";

	m_defaults["DOT_PATH"]="dot";

	m_defaults["HTML_HEAD_EXTRA_CODE"]="";
	m_defaults["HTML_TAIL_EXTRA_CODE"]="";
	m_defaults["HTML_MAINPAGE_EXTRA_CODE"]="";
}

bool CMooxygenOptions::loadFromFile(const string &f) /// false on error
{
	m_data.clear();

	ifstream fi;
	fi.open(f.c_str());
	if (!fi.is_open())
	{
		cerr << "ERROR: Cannot open file: " << f << endl;
		return false;
	}

	size_t  nLin=0;
	while (!fi.eof() && !fi.fail())
	{
		string lin;
		std::getline(fi,lin);
		nLin++;

		// Pre-Process line.
		// Remove comments "//", except when they're part of an URI
		size_t p = lin.find("//");
		if (p!=string::npos && (p==0 || lin[p-1]!=':' ) )
			lin=lin.substr(0,p);
		lin = trim(lin);

		if (lin.empty()) continue;

		// Process line:
		p = lin.find("=");
		if (p==string::npos)
		{
			cout <<f<<"("<< nLin<<") : error : line has not the format KEY = VAL "<< endl;
			return false;
		}
		const string key = trim(lin.substr(0,p));
		const string val = trim(lin.substr(p+1));
		if (m_data.find(key)!=m_data.end())
		{
			cout <<f<<"("<< nLin<<") : error : key " << key << " already has a value."<< endl;
			return false;
		}
		m_data[key]=val;
	}

	return true;
}

string CMooxygenOptions::get(const string &f) const /// Empty if not found
{
	Str2StrMap::const_iterator it = m_data.find(f);
	if (it!=m_data.end())
		return it->second;

	it = m_defaults.find(f);
	if (it!=m_defaults.end())
		return it->second;

	return string(); // Not found.
}

bool CMooxygenOptions::saveTemplateFile(const string &f) const /// false on error
{
	if (pathExists(f))
	{
		cerr << "ERROR: Output file already exists: " << f << endl;
		return false;
	}

	ofstream  fo;
	fo.open(f.c_str());
	if (!fo.is_open())
	{
		cerr << "ERROR: Cannot open file for writting: " << f << endl;
		return false;
	}

	fo << "// Template Mooxyfile project" << endl;
	fo << "//  For more help:  http://code.google.com/p/mooxygen/" << endl;
	fo << "//-----------------------------------------------------------" << endl;

	for( Str2StrMap::const_iterator it=m_defaults.begin();it!=m_defaults.end();++it)
		fo << rightPad(it->first,20) << " = " << it->second << endl;

	return true;
}
