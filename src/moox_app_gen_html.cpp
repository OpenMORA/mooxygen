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
	cout << "Generator for HTML..." << endl;

	const string oPath = opts.get("OUT_HTML_PATH");
	mooxygen::createDirectory(oPath);
	if (!pathExists(oPath))
	{
		cerr << "Error: Cannot create directory for HTML output: " << oPath << endl;
		return false;
	}
	chdir(oPath.c_str());

	updateAllURLs();

	// Generate INDEX.HTML
	// =======================================================
	{
		ofstream f("index.html");
		if (!f.is_open()) throw runtime_error("Error: Cannot create output HTML file.");

		f << "<html><head><title>PROJECT NAME - Mooxygen</title></head>" << endl;
		f << "<body>" << endl;
		f << generateGraphHTML_PNG("","", "graph_global" ); // The global graph
		f << "</body></html>" << endl;
	}

	// Generate MODULES.HTML
	// =======================================================
	{
		ofstream f("modules.html");
		if (!f.is_open()) throw runtime_error("Error: Cannot create output HTML file.");

		f << "<html><head><title>PROJECT NAME - Mooxygen</title></head>" << endl;
		f << "<body>" << endl;
		//...
		f << "</body></html>" << endl;
	}

	// Generate module_<NAME>.html
	// =======================================================
	for (TModList::iterator i=mods.begin();i!=mods.end();++i)
	{
		ofstream f(i->second.URL.c_str());
		if (!f.is_open()) throw runtime_error("Error: Cannot create output HTML file.");

		f << "<html><head><title>PROJECT NAME - Mooxygen</title></head>" << endl;
		f << "<body>" << endl;
		f << generateGraphHTML_PNG(i->first,"", fileNameStripInvalidChars(i->first) );
		f << "</body></html>" << endl;
	}

	// Generate var_<NAME>.html
	// =======================================================
	for (TVarList::iterator i=vars.begin();i!=vars.end();++i)
	{
		ofstream f(i->second.URL.c_str());
		if (!f.is_open()) throw runtime_error("Error: Cannot create output HTML file.");

		f << "<html><head><title>PROJECT NAME - Mooxygen</title></head>" << endl;
		f << "<body>" << endl;
		f << generateGraphHTML_PNG("",i->first, fileNameStripInvalidChars(i->first) );
		f << "</body></html>" << endl;
	}

	return true;
}


void TApplication::updateAllURLs()
{
	StrSet   allURLs; // To assure they're unique.

	for (TModList::iterator i=mods.begin();i!=mods.end();++i)
	{
		int n=0;
		const string good_name = fileNameStripInvalidChars( i->first );
		string nam;
		for (;;)
		{
			nam = string("module_")+good_name;
			if (n>0) nam += format("%i",n);
			n++;
			if (allURLs.find(nam)==allURLs.end())
			{
				allURLs.insert(nam);
				break;
			}
		}
		i->second.URL = nam + string(".html");
	}

	for (TVarList::iterator i=vars.begin();i!=vars.end();++i)
	{
		int n=0;
		const string good_name = fileNameStripInvalidChars( i->first );
		string nam;
		for (;;)
		{
			nam = string("var_")+good_name;
			if (n>0) nam += format("%i",n);
			n++;
			if (allURLs.find(nam)==allURLs.end())
			{
				allURLs.insert(nam);
				break;
			}
		}
		i->second.URL = nam + string(".html");
	}
}


string TApplication::generateGraphHTML_PNG(
	const string &only_mod,
	const string &only_var,
	const string &GRAPH_NAME)
{
	static const string FORMAT_MODS = " node [shape=box,style=filled, fillcolor=yellow,fontsize=10];";
	static const string FORMAT_VARS = " node [shape=ellipse,style=filled, fillcolor=red,fontsize=8];";

	cout << "Generating graph "  << GRAPH_NAME << "..." << endl;

	ofstream f("temp.dot");
	if (!f.is_open()) 
		throw runtime_error("Error: Cannot create temporary file for DOT");

	f << "digraph " << GRAPH_NAME <<   " {" << endl;

	if (only_mod.empty() && only_var.empty())
	{
		// Modules: --------------------
		f << FORMAT_MODS << endl;
		for (TModList::iterator i=mods.begin();i!=mods.end();++i)
			f << i->first << " [URL=\"" << i->second.URL <<   "\"];" << endl;
		// Vars:
		f << FORMAT_VARS << endl;
		for (TVarList::iterator i=vars.begin();i!=vars.end();++i)
			f << i->first << " [URL=\"" << i->second.URL <<   "\"];" << endl;
		// Publishes:
		for (TModList::iterator i=mods.begin();i!=mods.end();++i)
			for (StrSet::const_iterator p=i->second.publishes.begin();p!=i->second.publishes.end();++p)
				f << i->first << "->" << *p << ";" << endl;
		// Subscribes:
		for (TModList::iterator i=mods.begin();i!=mods.end();++i)
			for (StrSet::const_iterator p=i->second.subscribes.begin();p!=i->second.subscribes.end();++p)
				f << *p << "->" << i->first << ";" << endl;
	}
	else if (!only_mod.empty())
	{
		// Only one module: "only_mod" --------------------
		StrSet  myVars;

		for (StrSet::const_iterator p=mods[only_mod].publishes.begin();p!=mods[only_mod].publishes.end();++p)
			myVars.insert(*p);
		for (StrSet::const_iterator p=mods[only_mod].subscribes.begin();p!=mods[only_mod].subscribes.end();++p)
			myVars.insert(*p);
		
		// Modules:
		StrSet  myMods;

		f << FORMAT_MODS << endl;
		for (TModList::iterator i=mods.begin();i!=mods.end();++i)
		{
			bool isIn=false;
			for (StrSet::const_iterator p=i->second.publishes.begin();!isIn && p!=i->second.publishes.end();++p)
				if (myVars.count(*p)!=0)
					isIn=true;
			for (StrSet::const_iterator p=i->second.subscribes.begin();!isIn && p!=i->second.subscribes.end();++p)
				if (myVars.count(*p)!=0)
					isIn=true;
			if (isIn)
			{
				myMods.insert(i->first);

				f << i->first << " [";
				if (CompareCI(only_mod,i->first)) 
						f << "penwidth=4, ";
				else	f << "fillcolor=\"white\",";
				f << "URL=\"" << i->second.URL <<   "\"];" << endl;
			}
		}

		// Vars:
		f << FORMAT_VARS << endl;
		for (StrSet::iterator i=myVars.begin();i!=myVars.end();++i)
			f << *i<< " [URL=\"" << vars[*i].URL <<   "\"];" << endl;

		// Publishes:
		for (TModList::iterator i=mods.begin();i!=mods.end();++i)
			for (StrSet::const_iterator p=i->second.publishes.begin();p!=i->second.publishes.end();++p)
				if (myMods.count(i->first) && myVars.count(*p))
				{
					f << i->first << "->" << *p;
					if (!CompareCI(i->first,only_mod))
						f << "[style=dashed]";
					f << ";" << endl;
				}
		// Subscribes:
		for (TModList::iterator i=mods.begin();i!=mods.end();++i)
			for (StrSet::const_iterator p=i->second.subscribes.begin();p!=i->second.subscribes.end();++p)
				if (myMods.count(i->first) && myVars.count(*p))
				{
					f << *p << "->" << i->first;
					if (!CompareCI(i->first,only_mod))
						f << "[style=dashed]";
					f << ";" << endl;
				}
	}
	else if (!only_var.empty())
	{
		// Only one variable: "only_var" --------------------
		StrSet  myNodes;
		for (TModList::iterator i=mods.begin();i!=mods.end();++i)
		{
			for (StrSet::const_iterator p=i->second.publishes.begin();p!=i->second.publishes.end();++p)
				if (CompareCI(*p,only_var))
				{
					myNodes.insert(i->first);
					break;
				}
			for (StrSet::const_iterator p=i->second.subscribes.begin();p!=i->second.subscribes.end();++p)
				if (CompareCI(*p,only_var))
				{
					myNodes.insert(i->first);
					break;
				}
		}
		
		// Modules:
		f << FORMAT_MODS << endl;
		for (StrSet::iterator i=myNodes.begin();i!=myNodes.end();++i)
			f << *i << " [fillcolor=\"white\", URL=\"" << mods[*i].URL <<"\"];" << endl;

		// Vars:
		f << FORMAT_VARS << endl;
		f << only_var << " [URL=\"" << vars[only_var].URL << "\"];" << endl;

		// Publishes:
		f << FORMAT_MODS << endl;
		for (TModList::iterator i=mods.begin();i!=mods.end();++i)
			for (StrSet::const_iterator p=i->second.publishes.begin();p!=i->second.publishes.end();++p)
				if (CompareCI(only_var, *p))
					f << i->first << "->" << *p << ";" << endl;
		// Subscribes:
		for (TModList::iterator i=mods.begin();i!=mods.end();++i)
			for (StrSet::const_iterator p=i->second.subscribes.begin();p!=i->second.subscribes.end();++p)
				if (CompareCI(only_var, *p))
					f << *p << "->" << i->first  << ";" << endl;
	}

	f << "}" << endl;

	const string png_fil = string("var_") + GRAPH_NAME + string(".png");
	const string map_fil = GRAPH_NAME + string(".map");

	int ret = system(format("dot -Tpng -o %s temp.dot",png_fil.c_str()).c_str());
	if (ret!=0) throw runtime_error("ERROR executing external tool DOT.");
	  
	ret = system(format("dot -Tcmapx -o %s temp.dot",map_fil.c_str()).c_str());
	if (ret!=0) throw runtime_error("ERROR executing external tool DOT.");

	string  cod = "<img src=\"" + png_fil + "\" border=\"0\" usemap=\"#" + GRAPH_NAME + "\" />\n";
	cod+= readTextFile(map_fil);
	cod+= "\n";

	// Erase temp files:
	remove("temp.dot");
	remove(map_fil.c_str());

	return cod;
}
