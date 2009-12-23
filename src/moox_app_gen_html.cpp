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

#include "moox_app.h"

#include <direct.h>

using namespace std;
using namespace mooxygen;

bool TApplication::generateOutput_HTML()
{
	const string oPath = opts.get("OUT_HTML_PATH");
	mooxygen::createDirectory(oPath);
	if (!pathExists(oPath))
	{
		cerr << "Error: Cannot create directory for HTML output: " << oPath << endl;
		return false;
	}

	cout << "Generator for HTML..." << endl;

	chdir(oPath.c_str());

	// Generate the GLOBAL graph -------------------------------
	{
		ofstream f("temp.dot");
		if (!f.is_open()) {
			cerr << "Error: Cannot create temporary file for DOT" << endl;
			return false;
		}

		f << "digraph " << "GRAPH_GLOBAL" <<   " {" << endl;
		
		// Modules:
		f << " node [shape=box,style=filled, fillcolor=yellow,fontsize=10];" << endl;
		for (TModList::iterator i=mods.begin();i!=mods.end();++i)
			f << i->first << " [URL=\"module_" << i->first <<   ".html\"];" << endl;

		// Vars:
		f << " node [shape=ellipse,style=filled, fillcolor=red,fontsize=8];" << endl;
		for (TVarList::iterator i=vars.begin();i!=vars.end();++i)
			f << i->first << " [URL=\"var_" << i->first <<   ".html\"];" << endl;

		// Publishes:
		for (TModList::iterator i=mods.begin();i!=mods.end();++i)
			for (StrSet::const_iterator p=i->second.publishes.begin();p!=i->second.publishes.end();++p)
				f << i->first << "->" << *p << ";" << endl;
		
		// Subscribes:
		for (TModList::iterator i=mods.begin();i!=mods.end();++i)
			for (StrSet::const_iterator p=i->second.subscribes.begin();p!=i->second.subscribes.end();++p)
				f << *p << "->" << i->first << ";" << endl;

		f << "}" << endl;
	}
	int ret = system("dot -Tpng -o global.png temp.dot");
	ret = system("dot -Tcmapx -o global.map temp.dot");

	// Generate INDEX.HTML
	// =======================================================
	{
		ofstream f("index.html");
		if (!f.is_open()) { cerr << "Error: Cannot create output HTML file. " << endl; return false; }
		f << "<html><head><title>PROJECT NAME - Mooxygen</title></head>" << endl;
		f << "<body>" << endl;

		f << "<img src=\"global.png\" border=\"0\" usemap=\"#GRAPH_GLOBAL\" />" << endl;
		f << readTextFile("global.map") << endl;

		f << "</body></html>" << endl;
	}



	return true;
}