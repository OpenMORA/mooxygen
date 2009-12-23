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

using namespace std;
using namespace mooxygen;


set<string,ci_less>  validExts;

bool recursiveDirExplorer(
	const string dir, 
	set<string> &lstDirs,
	list<TFileInfo> &lstFiles )
{
	TDirListing dirlist;
	if (!mooxygen::dirExplorer(dir, dirlist))
	{
		cerr << "Error scanning directory " << dir << endl;
		return false;
	}

	for (TDirListing::const_iterator it=dirlist.begin();it!=dirlist.end();it++)
	{
		if (it->isDir)
		{
			if (it->name[0]!='.')
			{
				// Recursive:
				if (!recursiveDirExplorer(it->wholePath,lstDirs,lstFiles)) return false;
			}
		}
		else
		{
			// It's a file:
			size_t p=it->wholePath.find_last_of(".");
			if (p!=string::npos && p>0)
			{
				const string ext = it->wholePath.substr(p+1);
				if (validExts.count(ext))
				{
					lstFiles.push_back(*it);
					lstDirs.insert(dir);
				}
			}
		}
	}
	return true;
}

bool TApplication::scanForSourceFiles()
{
	if (validExts.empty())
	{
		validExts.insert("cpp");
		validExts.insert("cxx");
		validExts.insert("h");
		validExts.insert("hpp");
	}

	string root_path = convertBackslashes( trim(opts.get("INPUT_PATH")) );
	
	if (!pathExists(root_path))
	{
		cerr << "Project root path does not exist: " << root_path << endl;
		return false;
	}

	return recursiveDirExplorer( root_path,lstDirectories,lstSourceFiles);
}

bool TApplication::parseSourceFiles()
{

	return true;
}

