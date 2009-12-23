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


#ifndef _DIR_EXPLORER_H
#define _DIR_EXPLORER_H

#if defined(_WIN32) || defined(_WIN32_)  || defined(WIN32)
    #define IS_WINDOWS
#else
#if defined(unix) || defined(__unix__) || defined(__unix)
    #define IS_LINUX
#else
    #error Unsupported Implementation (Found neither _WIN32_ nor __unix__)
#endif
#endif

#include <cstdlib>
#include <cstdio>
#include <ctime>

#include <string>
#include <vector>
#include <deque>
#include <set>
#include <map>
#include <functional>
#include <algorithm>

#include <iostream>
#include <fstream>


namespace mooxygen
{
	using namespace std;


	/** Returns true if path exists and we have permissions to read it.
	  */
	bool pathExists(const std::string &path);

	/** Returns true if path is a directory.
	  */
	bool isDirectory( const std::string &fileName);

	/** Exits the program with a given error code
	  */
	void exitProgram(int errCode, bool waitKey=true);

	/** copyFileAttributes (WIN only) - Returns true on success, false on error.
	  */
	bool copyFileAttributes(const string& org, const string &trg);

	string format(const char *fmt, ...);	//! A sprintf-like function for std::string 

	/** Returns true on success (retVal==0), false on error (retVal!=0).
	  */
	bool SHOW_ERROR_IF_ANY( int retVal );


	/** copyFile - Copy a file to a target filename (the source & target MUST be filenames, not directories) 
	  *  This function preserves the original file timestamps - Returns true on success, false on error.
	  */
	bool copyFile( const std::string &org, const std::string &trg );

	/** copyDirectory - Recursively copy files and directories (the source & target MUST be directories - if target directory does not exist it will be created) 
	  *  This function preserves the original file timestamps - Returns true on success, false on error.
	  *  If "copyCount" is not NULL, its value will be INCREMENTED (initialize before calling!) by the number of copied files/directories.
	  */
	bool copyDirectory( const std::string &org, const std::string &trg, long *copyCount = NULL );

	/**  deleteFileOrDir - Delete a file or a directory - Returns true on success, false on error.
	  *   If "deleteCount" is not NULL, it returns the total amount of files/directories deleted in the whole tree (INITIALIZE THIS VARIABLE TO ZERO BEFORE CALL!!).
	  */
	bool deleteFileOrDir( const std::string &filename, long * deleteCount = NULL );

	/** Creates a new directory (does not fail if if already exists) - In linux, the directory is created with RWX permisions for everyone - Returns true on success, false on error.
	  */
	bool createDirectory( const std::string &path );

	/** changeFileTimes - Returns true on success, false on error.
	  */
	bool changeFileTimes( const std::string &fileName, time_t accessTime, time_t modifTime );

	struct TFileInfo
	{
		std::string     name,wholePath;
		time_t          accessTime,modTime;
		bool            isDir, isSymLink;
	};

	typedef std::deque<TFileInfo> TDirListing;

	/** Explores a given path and return the list of its contents. Returns true if OK, false on any error.
	  */
	bool dirExplorer( const std::string &inPath, TDirListing &outList );

	/** Waits for a key
	  */
	void waitForKey();

	/** Returns a string representation of a time ellapse
	  */
	string formatTime( long secs );


	/** A case-insensitive comparator struct for usage within STL containers, eg: map<string,string,ci_less>
	  */
	struct ci_less : std::binary_function<std::string,std::string,bool>
	{
		// case-independent (ci) compare_less binary function
		struct nocase_compare : public std::binary_function<char,char,bool> {
			bool operator()(const char c1, const char c2) const { return tolower(c1)<tolower(c2); }
		};
		bool operator() (const std::string & s1, const std::string & s2) const {
			return std::lexicographical_compare(s1.begin(),s1.end(), s2.begin(),s2.end(), nocase_compare());
		}
	}; // end of ci_less

	typedef map<string,string,ci_less> Str2StrMap;
	typedef set<string,ci_less> StrSet;

	/** Right pad with spaces */
	std::string rightPad(const std::string &str, const size_t total_len);

	/**  Removes leading and trailing spaces */
	std::string trim(const std::string &str);

	string convertBackslashes(const string &s); /// Convert \\ -> /

	/** Returns a lower-case version of a string.
	  * \sa lowerCase  */
	std::string  upperCase(const std::string& str);

	/** Returns an upper-case version of a string.
	  * \sa upperCase  */
	std::string  lowerCase(const std::string& str);

	string readTextFile(const string &filename);

	void  tokenize(
		const std::string			&inString,
		const std::string			&inDelimiters,
		std::vector<std::string>	&outTokens );

	#define SHOW_AND_LOG_ERROR(M) { cerr << (M) << endl;  }
	#define SHOW_AND_LOG_ERROR2(M1,M2) { cerr << (M1) << (M2) << endl;  }
	#define SHOW_AND_LOG_ERROR2_NOENDL(M1,M2) { cerr << (M1) << (M2);  }
	#define SHOW_AND_LOG_ERROR4(M1,M2,M3,M4) { cerr << (M1) << (M2) << (M3) << (M4) << endl;  }

	#define SHOW_AND_LOG_MSG(M) { cout << (M) << endl;  }
	#define SHOW_AND_LOG_MSG_NOENDL(M) { cout << (M);  }
	#define SHOW_AND_LOG_MSG2(M1,M2) { cout << (M1) << (M2) << endl;  }
	#define SHOW_AND_LOG_MSG3(M1,M2,M3) { cout << (M1) << (M2) << (M3) << endl;  }
	#define SHOW_AND_LOG_MSG4(M1,M2,M3,M4) { cout << (M1) << (M2) << (M3) << (M4) << endl;  }

	#define SHOW_AND_LOG_MSG5_NOENDL(M1,M2,M3,M4,M5) { cout << (M1) << (M2) << (M3) << (M4) << (M5); }

};

#endif

