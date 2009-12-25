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
#include "moox_version.h"

#ifdef IS_WINDOWS
	#include <direct.h>
#endif

using namespace std;
using namespace mooxygen;

#define MOOS_URL "http://www.robots.ox.ac.uk/~mobile/MOOS/wiki/pmwiki.php"

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

	const string  PROJECT_NAME = opts.get("PROJECT");
	const string  html_head =format(
		"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n"
		"<html><head><meta http-equiv=\"Content-Type\" content=\"text/html;charset=iso-8859-1\">\n"
		"<title>%s</title>\n"
		"<link href=\"tabs.css\" rel=\"stylesheet\" type=\"text/css\">\n"
		"%s\n"
		"<!-- Page generated automatically by Mooxygen " MOOXYGEN_VERSION " -->\n"
		"<body>\n", PROJECT_NAME.c_str(), opts.get("HTML_HEAD_EXTRA_CODE").c_str());
	const string  html_tail = format(
		"\n<br><br><hr><small>Page generated by <a href=\"http://code.google.com/p/mooxygen/\" >Mooxygen " MOOXYGEN_VERSION "</a> at %s</small>\n<br>"
		"<div align=\"right\">"
		" <a href=\"http://validator.w3.org/check?uri=referer\">"
		" <img border=\"0\" src=\"http://www.w3.org/Icons/valid-html401-blue\" alt=\"Valid HTML 4.01 Transitional\" height=\"31\" width=\"88\"></a>\n"
		"</div>\n"
		"%s\n"
		"</body></html>", nowAsString().c_str(), opts.get("HTML_TAIL_EXTRA_CODE").c_str());

	// Common files
	// =======================================================
	saveResourceToFile("tabs.css","tabs.css");
	saveResourceToFile("tab_b.gif","tab_b.gif");
	saveResourceToFile("tab_l.gif","tab_l.gif");
	saveResourceToFile("tab_r.gif","tab_r.gif");

	vector<pair<string,string> >  tabs;
	tabs.push_back(make_pair<string,string>("Global overview","index.html"));
	tabs.push_back(make_pair<string,string>("All modules","modules.html"));
	tabs.push_back(make_pair<string,string>("All variables","variables.html"));
	//tabs.push_back(make_pair<string,string>("Source files","files.html"));

	// Generate INDEX.HTML
	// =======================================================
	{
		ofstream f("index.html");
		if (!f.is_open()) throw runtime_error("Error: Cannot create output HTML file.");

		f << html_head;
		f << generateTabsHTML(tabs,0);
		f << "<br>\n";
		f << "<center><h3>"<< PROJECT_NAME <<"</h3></center><br><br>\n";
		f << "This is the overview of all the <a href=\"" MOOS_URL "\" >MOOS</a> modules and variables for the project ";
		f << "<i>"<< PROJECT_NAME <<"</i>. Click on any module or variable in the graph to see more details.<br>\n";
		f << "You can also use the tabs above to navigate among the documentation.\n";
		f << "<br> <br>";
		f << "<div align=\"center\"><b>GLOBAL VIEW:</b></div> <br>";
		f << generateGraphHTML_PNG("","", "graph_global" ); // The global graph
		f << "<br><br><br><div align=\"left\"><b>Legend:</b><br>\n";
		f << "<ul>";
		f << "<li><b>Modules:</b> Rectangles. </li>\n";
		f << "<li><b>Variables:</b> Ellipses. </li>\n";
		f << "</ul></div>\n";
		f << html_tail;
	}

	// Generate MODULES.HTML
	// =======================================================
	{
		ofstream f("modules.html");
		if (!f.is_open()) throw runtime_error("Error: Cannot create output HTML file.");

		f << html_head;
		f << generateTabsHTML(tabs,1);
		f << "<br>List of all modules:<br>\n";
		f << generateHTMLTableOfModules();
		f << html_tail;
	}

	// Generate module_<NAME>.html
	// =======================================================
	for (TModList::iterator i=mods.begin();i!=mods.end();++i)
	{
		ofstream f(i->second.URL.c_str());
		if (!f.is_open()) throw runtime_error("Error: Cannot create output HTML file.");

		f << html_head;
		f << generateTabsHTML(tabs,1);
		f << "<br><center><h3> Module: ";
		f << validTextHTML(i->first);
		f << "</h3></center><br>\n";
		f << "<u>Module info:</u><br>\n";
		f << generateHTMLTableOfModules(i->first);
		f << "<br>\n";
		f << "<u>Module graph:</u><br>\n";
		f << generateGraphHTML_PNG(i->first,"", fileNameStripInvalidChars(i->first) );
		f << "<br><u>Detailed description:</u><br>\n";
		f << validTextHTML(i->second.getDesc());
		f << html_tail;
	}

	// Generate variables.html
	// =======================================================
	{
		ofstream f("variables.html");
		if (!f.is_open()) throw runtime_error("Error: Cannot create output HTML file.");

		f << html_head;
		f << generateTabsHTML(tabs,2);
		f << "<br>List of all variables:<br>\n";
		f << generateHTMLTableOfVariables();
		f << html_tail;
	}

	// Generate var_<NAME>.html
	// =======================================================
	for (TVarList::iterator i=vars.begin();i!=vars.end();++i)
	{
		ofstream f(i->second.URL.c_str());
		if (!f.is_open()) throw runtime_error("Error: Cannot create output HTML file.");

		f << html_head;
		f << generateTabsHTML(tabs,2);

		f << "<br><center><h3> Variable: ";
		f << validTextHTML(i->first);
		f << "</h3></center><br>\n";
		f << "<u>Variable info:</u><br>\n";
		f << generateHTMLTableOfVariables(i->first);
		f << "<br>\n";
		f << "<u>Variable graph:</u><br>\n";
		f << generateGraphHTML_PNG("",i->first, fileNameStripInvalidChars(i->first) );
		f << "<br><u>Detailed description:</u><br>\n";
		f << validTextHTML(i->second.getDesc());
		f << html_tail;
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

	string  cod = "<div align=\"center\"> <img src=\"" + png_fil + "\" border=\"0\" usemap=\"#" + GRAPH_NAME + "\" alt=\""+GRAPH_NAME+"\"> </div>\n";
	cod+= replaceInString(readTextFile(map_fil),"/>",">");
	cod+= "\n";

	// Erase temp files:
	remove("temp.dot");
	remove(map_fil.c_str());

	return cod;
}

string TApplication::generateTabsHTML(
	const vector<pair<string,string> > &tabs,
	size_t active_idx)
{
	string ret;
	ret+=
		"<div class=\"tabs\">\n"
		" <ul>\n";
	for (size_t i=0;i<tabs.size();i++)
	{
		if (i==active_idx)
				ret+="  <li class=\"current\">";
		else 	ret+="  <li>";

		ret+=format("<a href=\"%s\"><span>%s</span></a></li>\n",tabs[i].second.c_str(),tabs[i].first.c_str());
	}

	ret+=" </ul>\n";
	ret+="</div><br>\n";
	return ret;
}

string TApplication::generateHTMLTableOfModules(const string &mod)
{
	string ret;

	ret+="<br> <div align=\"center\" ><table class=\"mooxygen\" width=\"95%\" >\n";
	ret+="<tr>\n";
	ret+=" <td align=\"center\" bgcolor=\"grey\"> <b>Module name</b> </td>\n";
	ret+=" <td align=\"center\" bgcolor=\"grey\"> <b>Short description</b> </td>\n";
	ret+=" <td align=\"center\" bgcolor=\"grey\"> <b>Publishes</b> </td>\n";
	ret+=" <td align=\"center\" bgcolor=\"grey\"> <b>Subscribes</b> </td>\n";
	ret+="</tr>\n";

	for (TModList::iterator i=mods.begin();i!=mods.end();++i)
	{
		if (mod.empty() || i->first==mod)
		{
			ret+="<tr>\n";
			ret+=" <td align=\"center\"> ";
			ret+="<a href=\"";
			ret+=i->second.URL;
			ret+="\" > <small>";
			ret+=validTextHTML(i->first);
			ret+="</small> </a></td>\n";

			// Short desc:
			ret+=" <td align=\"center\"> <small>";
			if (i->second.short_desc.empty())
					ret+="(no description)";
			else	ret+=validTextHTML(i->second.short_desc);
			ret+= "</small> </td>\n";

			// publishes:
			ret+=" <td align=\"center\"> <small>";
			for (StrSet::const_iterator s=i->second.publishes.begin();s!=i->second.publishes.end();++s)
			{
				if (s!=i->second.publishes.begin())
					ret+=", ";
				ret+= "<a href=\""+ vars[*s].URL +"\" >" + validTextHTML(*s) + "</a> ";
			}
			ret+="</small> </td>\n";

			// subscribes:
			ret+=" <td align=\"center\"> <small>";
			for (StrSet::const_iterator s=i->second.subscribes.begin();s!=i->second.subscribes.end();++s)
			{
				if (s!=i->second.subscribes.begin())
					ret+=", ";
				ret+= "<a href=\""+ vars[*s].URL +"\" >" + validTextHTML(*s) + "</a> ";
			}
			ret+="</small> </td>\n";

			ret+="</tr>\n";
		}
	}
	ret+="</table> </div>\n";

	return ret;
}

string TApplication::generateHTMLTableOfVariables(const string &var)
{
	string ret;

	ret+="<br> <div align=\"center\" ><table class=\"mooxygen\" width=\"95%\" >\n";
	ret+="<tr>\n";
	ret+=" <td align=\"center\" bgcolor=\"grey\"> <b>Variable name</b> </td>\n";
	ret+=" <td align=\"center\" bgcolor=\"grey\"> <b>Short description</b> </td>\n";
	ret+=" <td align=\"center\" bgcolor=\"grey\"> <b>Who publishes it?</b> </td>\n";
	ret+=" <td align=\"center\" bgcolor=\"grey\"> <b>Who subscribes to it?</b> </td>\n";
	ret+="</tr>\n";

	for (TVarList::iterator i=vars.begin();i!=vars.end();++i)
	{
		if (var.empty() || i->first==var)
		{
			ret+="<tr>\n";
			ret+=" <td align=\"center\"> ";
			ret+="<a href=\"";
			ret+=i->second.URL;
			ret+="\" > <small>";
			ret+=validTextHTML(i->first);
			ret+="</small> </a></td>\n";

			// Short desc:
			ret+=" <td align=\"center\"> <small>";
			if (i->second.short_desc.empty())
					ret+="(no description)";
			else	ret+=validTextHTML(i->second.short_desc);
			ret+= "</small> </td>\n";

			// published:
			StrSet  modsPub;
			ret+=" <td align=\"center\"> <small>";
			for (TModList::const_iterator m=mods.begin();m!=mods.end();++m)
				for (StrSet::const_iterator s=m->second.publishes.begin();s!=m->second.publishes.end();++s)
					if (*s==i->first)
						modsPub.insert(m->first);
			for (StrSet::const_iterator s=modsPub.begin();s!=modsPub.end();++s)
				ret+= "<a href=\""+ mods[*s].URL +"\" >" + validTextHTML(*s) + "</a> ";
			ret+="</small> </td>\n";

			// subscribed to:
			StrSet  modsSub;
			ret+=" <td align=\"center\"> <small>";
			for (TModList::const_iterator m=mods.begin();m!=mods.end();++m)
				for (StrSet::const_iterator s=m->second.subscribes.begin();s!=m->second.subscribes.end();++s)
					if (*s==i->first)
						modsSub.insert(m->first);
			for (StrSet::const_iterator s=modsSub.begin();s!=modsSub.end();++s)
				ret+= "<a href=\""+ mods[*s].URL +"\" >" + validTextHTML(*s) + "</a> ";
			ret+="</small> </td>\n";

			ret+="</tr>\n";
		}
	}
	ret+="</table> </div>\n";

	return ret;
}

string TApplication::TModuleInfo::getDesc() const
{
	if (desc.empty()) return "(no description)";
	string s;
	for (list<string>::const_iterator i=desc.begin();i!=desc.end();++i)
	{
		s+=*i;
		s+="<br>\n";
	}
	return s;
}

string TApplication::TVariableInfo::getDesc() const
{
	if (desc.empty()) return "(no description)";
	string s;
	for (list<string>::const_iterator i=desc.begin();i!=desc.end();++i)
	{
		s+=*i;
		s+="<br>\n";
	}
	return s;
}
