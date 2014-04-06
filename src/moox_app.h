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
		string mainPage;  //!< The text of the main page (if not redefined, there is a predefined short text)
		typedef list<TFileInfo> TSourcesList;

        // Modules:
		TSourcesList lstSourceFiles;
		set<string> lstDirectories;

        // Mission files:
		TSourcesList lstMissionFiles;
		set<string> lstMissionDirectories;


		struct TModuleInfo
		{
			vector<TFileInfo>	files;
			string	short_desc;
			string	desc;
			string	URL;   //!< "module_<SANITAZED_NAME>.html"
			string	TODO;
			string	changeLog;

			StrSet	publishes;  // Var names
			StrSet	subscribes; // Var names
			StrSet	commands;   // Names of accepted commands
			map<string,string,ci_less>  params; // mission file parameters: name -> description

			string getDesc() const;
			string getTODO() const;
			string getChangeLog() const;
		};
		struct TMissionFileInfo
		{
			string	URL;   //!< "module_<SANITAZED_NAME>.html"
			string  short_desc;
		    StrSet  modules; //<! Modules that appear here.
		    deque<string> contents; //!< HTML syntax-highlighted contents
		};
		struct TVariableInfo
		{
			string	short_desc;
			string	desc;
			string	URL;    //!< "module_<SANITAZED_NAME>.html"
			string getDesc() const;
		};
		struct TCommandInfoPerModule
		{
			string 	short_desc;
			string	desc;
		};
		struct TCommandInfo : public map<string,TCommandInfoPerModule,ci_less>
		{
			string URL;
		};

		struct TPipeLineInfo
		{
			string	URL;    //!< "pipeline_<SANITAZED_NAME>.html"
			StrSet  modules;
			string 	short_desc;
			string	desc;
		};

		typedef Str2ValueCIMap<TModuleInfo> 	TModList;
		typedef Str2ValueCIMap<TVariableInfo> 	TVarList;
		typedef Str2ValueCIMap<TCommandInfo> 	TCmdList; //! cmd name -> info for each module
		typedef std::map<std::string, TMissionFileInfo> TMissionList; 
		typedef Str2ValueCIMap<TPipeLineInfo> 	TPipelineList; 

		TModList mods;
		TVarList vars;
		TCmdList cmds;
		TMissionList missions; //!< Keys are file paths
		TPipelineList pipelines; //!< Keys are pipeline names


		// Methods ---------------------------
		bool scanForSourceFiles();  /// return false on error
		bool parseSourceFiles();    /// return false on error

		bool scanForMissionFiles();  /// return false on error
		bool parseMissionFiles();    /// return false on error

		bool generateOutputs();    /// return false on error

	private:
		string	m_root_path;
		string	m_root_path_missions;
		bool parseOneSourceFile( const TSourcesList::value_type fil );
		bool parseOneMissionFile( const TSourcesList::value_type fil );
		string getRelativePath(const string &f) const;
		string getMissionRelativePath(const string &f) const;

		void processCommentBlocks(const TSourcesList::value_type fil, const list<list<string> > &lins, const list<string> &autodetectedPublishVars );
		bool generateOutput_HTML();    /// return false on error
		void updateAllURLs();
		string generateHTMLTableOfModules(const string &mod=string());
		string generateHTMLTableOfVariables(const string &var=string());
		string generateHTMLTableOfMissions(const string &var=string());
		string generateHTMLTableOfPipelines(const string &var=string());
		

		string generateGraphHTML_PNG(
			const string &only_mod,
			const string &only_var,
			const string &GRAPH_NAME);

		static string generateTabsHTML(const vector<pair<string,string> > &tabs, size_t active_idx);
	};
}
#endif

