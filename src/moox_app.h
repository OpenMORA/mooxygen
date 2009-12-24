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

#ifndef moox_app_H
#define moox_app_H

#include "CMooxygenOptions.h"

#include <list>

namespace mooxygen
{
	using namespace std;

	/// All the information gathered by Dooxygen about one project.
	struct TApplication
	{
		// Options ---------------------------
		CMooxygenOptions  opts;

		// Data ------------------------------
		typedef list<TFileInfo> TSourcesList;

		TSourcesList lstSourceFiles;
		set<string> lstDirectories;

		struct TModuleInfo
		{
			vector<TFileInfo>	files;
			string			short_desc;
			list<string>	desc;
			string			URL;   //!< "module_<SANITAZED_NAME>.html"

			StrSet			publishes;  // Var names
			StrSet			subscribes; // Var names
		};
		struct TVariableInfo
		{
			string			short_desc;
			list<string>	desc;
			string			URL;    //!< "module_<SANITAZED_NAME>.html"
		};

		typedef map<string,TModuleInfo,ci_less> TModList;
		typedef map<string,TVariableInfo,ci_less> TVarList;

		TModList mods;
		TVarList vars;

	
		// Methods ---------------------------
		bool scanForSourceFiles();  /// return false on error
		bool parseSourceFiles();    /// return false on error
		bool generateOutputs();    /// return false on error

	private:
		string	m_root_path;
		bool parseOneSourceFile( const TSourcesList::value_type fil );
		string getRelativePath(const string &f);

		void processCommentBlocks(const TSourcesList::value_type fil, const list<list<string> > &lins );
		bool generateOutput_HTML();    /// return false on error
		void updateAllURLs();
		string generateGraphHTML_PNG(
			const string &only_mod,
			const string &only_var,
			const string &GRAPH_NAME);

	};
}
#endif

