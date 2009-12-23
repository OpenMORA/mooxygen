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


#define _CRT_SECURE_NO_DEPRECATE 1
#include "moox_utils.h"

#ifdef IS_LINUX
	#include <utime.h>
	#include <unistd.h>
	#include <errno.h>
	#define _access access
	#define _rmdir rmdir
#else
	#include <sys/utime.h>
	#include <io.h>
	#include <windows.h>
	#include <direct.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <queue>
#include <iostream>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>


using namespace std;

namespace mooxygen
{


void waitForKey()
{
#ifdef IS_WINDOWS
	system("pause");
#else
	system("bash -c \"read -p \\\"Press enter to exit...\\\"\"");
#endif
}


// ---------------------------------------------------------------
// Returns true if path exists:
// ---------------------------------------------------------------
bool pathExists(const std::string &path)
{
    return 0 == _access(path.c_str(), 0x00 ); // 0x00 = Check for existence only!
}

// ---------------------------------------------------------------
//  isDirectory
// ---------------------------------------------------------------
bool isDirectory( const std::string &fileName)
{
	// File times:
	struct stat statDat;
	if (stat( fileName.c_str(), &statDat ))
	{
		SHOW_AND_LOG_ERROR2("ERROR: [isDirectory] File does not exist: ",fileName.c_str());
		SHOW_ERROR_IF_ANY(-1);
		exitProgram(-1);
		return false;
	}
	else
		return 0!=(statDat.st_mode & 0x4000 );  //_S_IFDIR / S_ISDIR
}

#ifdef IS_WINDOWS
void showLastWindowsError()
{
	TCHAR str[33000];
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,GetLastError(),0,str,sizeof(str),NULL);
	cerr << str;
}
#endif

// ---------------------------------------------------------------
//
//  copyFile - Returns true on success, false on error.
//
// ---------------------------------------------------------------
bool copyFile( const std::string &org0, const std::string &trg0)
{
#ifdef IS_WINDOWS
	// WINDOWS --------------------------
	string org( convertBackslashes(org0) );
	string trg( convertBackslashes(trg0) );
#else
	string org( org0 );
	string trg( trg0 );
#endif

	// Check if source file exists and we have access to open it:
	FILE *f_src=fopen(org.c_str(),"rb");
	if (!f_src)
	{
		// Is source a directory?
		if ( !pathExists(org) )
		{
			SHOW_AND_LOG_ERROR2("ERROR: [copyFile] Source does not exist or permision denied!: ",org);
			return false;
		}
		else
		{
			// It does exist:
			if (isDirectory( org ) )
			{
				SHOW_AND_LOG_ERROR2("ERROR: [copyFile] Is source a directory?: ",org);
				return false;
			}
			else
			{
				SHOW_AND_LOG_ERROR2("ERROR: [copyFile] Source file exists but cannot open it... is file being used?: ",org);
				return false;
			}
		}
	}

	// Assure that "target" is not an existing directory:
	if ( pathExists(trg) )
	{
		if ( isDirectory(trg) )
		{
			SHOW_AND_LOG_ERROR2("ERROR: [copyFile] Target cannot be a directory: ",trg);
			fclose(f_src);
			return false;
		}
	}

	// Try to open the file for writting:
	FILE *f_trg=fopen(trg.c_str(),"wb");
	if (!f_trg)
	{
		if (!pathExists(trg))
		{
			// It does not exist and does not allow us to create it:
			SHOW_AND_LOG_ERROR2("ERROR: [copyFile] Cannot create target file: ",trg);
			fclose(f_src);
			return false;
		}
		else
		{
			// It exists, but cannot overwrite it:

#ifdef IS_WINDOWS
			// Try changing the permisions of the target file:
			DWORD dwProp = GetFileAttributes( trg.c_str() );
			if (dwProp==INVALID_FILE_ATTRIBUTES)
			{
				SHOW_AND_LOG_ERROR2("ERROR: [copyFile] Cannot get file attributes for target file, trying to remove a possible read-only attribute after first attempt of copy failed, for: ",trg);
				fclose(f_src);
				return false;
			}

			dwProp &= ~FILE_ATTRIBUTE_HIDDEN;
			dwProp &= ~FILE_ATTRIBUTE_READONLY;
			dwProp &= ~FILE_ATTRIBUTE_SYSTEM;

			if (!SetFileAttributes( trg.c_str(), dwProp ))
			{
				SHOW_AND_LOG_ERROR2("ERROR: [copyFile] Cannot set file attributes for target file, trying to remove a possible read-only attribute after first attempt of copy failed, for: ",trg);
				fclose(f_src);
				return false;
			}
			
			// Try again:
			f_trg=fopen(trg.c_str(),"wb");
			if (!f_trg)
			{
				SHOW_AND_LOG_ERROR2("ERROR: [copyFile] Cannot overwrite target file, even after changing file attributes! : ",trg);
				fclose(f_src);
				return false;
			}
#else
			// Try changing the permisions of the target file:
			if (chmod( trg.c_str(), S_IRWXU | S_IRGRP | S_IROTH ) )
			{
				SHOW_AND_LOG_ERROR4("ERROR: [copyFile] Cannot set file permissions for target file, trying to remove a possible read-only attribute after first attempt of copy failed, for: ",trg,". errno:",errno);
				fclose(f_src);
				return false;
			}

			// Try again:
			f_trg=fopen(trg.c_str(),"wb");
			if (!f_trg)
			{
				SHOW_AND_LOG_ERROR2("ERROR: [copyFile] Cannot overwrite target file, even after changing file permissions! : ",trg);
				fclose(f_src);
				return false;
			}
#endif
		}
	}

	// Ok, here we have both files open: Perform the copy:
	char	buf[66000];
	size_t	nBytes=0;
	while (0!= (nBytes=fread(buf,1,64*1024,f_src)) )
	{
		if (nBytes!=fwrite(buf,1,nBytes,f_trg))
		{
			SHOW_AND_LOG_ERROR2("ERROR: [copyFile] Error writing the contents of the target file: ",trg);
			fclose(f_src);
			fclose(f_trg);
			return false;
		}
	}


	// Close file handles:
	fclose(f_src);
	fclose(f_trg);

	// In Windows only, copy the file attributes:
	if (!copyFileAttributes(org,trg))
		return false;

	// Copy the file timestamps:
	struct stat statDat;
	if (stat( org.c_str(), &statDat ))
	{
		SHOW_AND_LOG_ERROR2("ERROR: [copyFile] Cannot get timestamps for the source file: ", org);
		return false;
	}


	if (!changeFileTimes( trg, statDat.st_atime, statDat.st_mtime ))
	{
		SHOW_AND_LOG_ERROR2("ERROR: [copyFile] Cannot change target file timestamps: ",trg);
		return false;
	}

#ifdef IS_LINUX
	// Assure the write permissions by the user:
	if (chmod( trg.c_str(), S_IRWXU | S_IRGRP | S_IROTH ) )
	{
		SHOW_AND_LOG_ERROR2("ERROR: [copyFile] Cannot change file permissions: ",trg);
		return false;
	}
#endif


	return true;
}

// -------------------------------------------------------------------------
//  copyFileAttributes (WIN only) - Returns true on success, false on error.
// --------------------------------------------------------------------------
bool copyFileAttributes(const string& org, const string &trg)
{
#ifdef IS_WINDOWS
	DWORD dwPropSrc = GetFileAttributes( org.c_str() );
	if (dwPropSrc==INVALID_FILE_ATTRIBUTES)
	{
		SHOW_AND_LOG_ERROR2("ERROR: [copyFileAttributes] Cannot get the file attributes for source file: ",org);
		return false;
	}
	DWORD dwPropTrg = GetFileAttributes( trg.c_str() );
	if (dwPropTrg==INVALID_FILE_ATTRIBUTES)
	{
		SHOW_AND_LOG_ERROR2("ERROR: [copyFileAttributes] Cannot get the file attributes for target file: ",trg);
		return false;
	}

	// Leave only those attributes which can be legally copied:
	// (Refer to: http://msdn2.microsoft.com/en-us/library/aa365535.aspx )
	dwPropSrc &= FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_NORMAL | \
				 FILE_ATTRIBUTE_NOT_CONTENT_INDEXED | FILE_ATTRIBUTE_OFFLINE | \
				 FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_TEMPORARY;

	// Copy them to the target attributes:
	dwPropTrg &= ~(FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_NORMAL | \
				   FILE_ATTRIBUTE_NOT_CONTENT_INDEXED | FILE_ATTRIBUTE_OFFLINE | \
				   FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_TEMPORARY);
	dwPropTrg |= dwPropSrc;

	// Set attributes of target file:
	if (!SetFileAttributes( trg.c_str(), dwPropTrg ))
	{
		SHOW_AND_LOG_ERROR2("ERROR: [copyFileAttributes] Cannot set file attributes for target file: ",trg);
		return false;
	}

	return true;
#else
	// Linux: Nothing to do!
	return true; 
#endif
}


// Returns true on success (retVal==0), false on error (retVal!=0).
bool SHOW_ERROR_IF_ANY( int retVal )
{
	if (retVal!=0)	perror(NULL);
	return retVal==0;
}

// ---------------------------------------------------------------
//  deleteFileOrDir - Deletes a file/dir - Returns true on success, false on error.
// ---------------------------------------------------------------
bool deleteFileOrDir( const std::string &filename, long *deleteCount )
{
#ifdef IS_WINDOWS
	// Windows only: Remove read-only protections:
	DWORD dwProp = GetFileAttributes( filename.c_str() );
	if (dwProp==INVALID_FILE_ATTRIBUTES)
	{
		if ( GetLastError()==ERROR_FILE_NOT_FOUND || GetLastError()==ERROR_PATH_NOT_FOUND )
				SHOW_AND_LOG_ERROR2("ERROR: [deleteFile] Path does not exist: ",filename)
		else	SHOW_AND_LOG_ERROR2("ERROR: [deleteFile] Cannot get the file attributes for file: ",filename);
		return false;
	}
	// Remove these ones:
	dwProp &= ~(FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM );
	if (!SetFileAttributes( filename.c_str(), dwProp ))
	{
		SHOW_AND_LOG_ERROR2("ERROR: [deleteFile] Cannot change file attributes for file: ",filename);
		return false;
	}
#endif

	if (isDirectory( filename ))
	{
		// Runs recursively:
		TDirListing		dirList;
		if (!dirExplorer(filename,dirList)) 
		{
			SHOW_AND_LOG_ERROR2("ERROR: [deleteFile] Cannot list contents of directory: ",filename);
			return false;
		}

		// Recursive delete:
		for (TDirListing::iterator it = dirList.begin();it!=dirList.end();it++)
			if (!deleteFileOrDir( it->wholePath, deleteCount)) return false;

		// Finally, remove the (empty) directory itself:
		if (deleteCount) (*deleteCount)++;
		return SHOW_ERROR_IF_ANY( _rmdir( filename.c_str() ) );
	}
	else
	{
		// Delete a single file:
		if (deleteCount) (*deleteCount)++;
		return SHOW_ERROR_IF_ANY( remove( filename.c_str() ) );
	}
}


// ---------------------------------------------------------------
//  changeFileTimes - Returns true on success, false on error.
// ---------------------------------------------------------------
bool changeFileTimes( const std::string &fileName, time_t accessTime, time_t modifTime )
{
    utimbuf auxTims;
    auxTims.actime  = accessTime;
    auxTims.modtime = modifTime;

	struct stat statDatBefore;
	if (!SHOW_ERROR_IF_ANY(stat( fileName.c_str(), &statDatBefore )))
	{
		SHOW_AND_LOG_ERROR2("ERROR: [changeFileTimes] Cannot access stat for file before modifing for ",fileName);
		return false;
	}

#ifdef IS_WINDOWS
	// Windows only: Remove read-only protection:
	DWORD dwProp = GetFileAttributes( fileName.c_str() );
	if (dwProp==INVALID_FILE_ATTRIBUTES)
	{
		SHOW_AND_LOG_ERROR2("ERROR: [changeFileTimes] Cannot get the file attributes for file: ",fileName);
		return false;
	}

	// Remove these ones:
	DWORD dwPropMod = dwProp & ~(FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM );

	if (!SetFileAttributes( fileName.c_str(), dwPropMod ))
	{
		SHOW_AND_LOG_ERROR2("ERROR: [changeFileTimes] Cannot change file attributes for file: ",fileName);
		return false;
	}
#endif

	if (!utime( fileName.c_str(), &auxTims ))
	{
		// DOUBLE CHECK!!
		struct stat statDat;
		if (!SHOW_ERROR_IF_ANY(stat( fileName.c_str(), &statDat )))
		{
			SHOW_AND_LOG_ERROR2("ERROR: [changeFileTimes] Cannot access stat for file while verifying ",fileName);
			return false;
		}
		if (difftime(statDat.st_mtime,modifTime)>2 )
		{
			SHOW_AND_LOG_ERROR2("ERROR: [changeFileTimes] Verifying timestamps do not match for file ",fileName);

			SHOW_AND_LOG_ERROR2_NOENDL("  accessTime read before set is : ", ctime(&statDatBefore.st_atime) );
			SHOW_AND_LOG_ERROR2_NOENDL("  accessTime was set to         : ", ctime(&accessTime) );
			SHOW_AND_LOG_ERROR2_NOENDL("  accessTime read after set is  : ", ctime(&statDat.st_atime) );

			SHOW_AND_LOG_ERROR2_NOENDL("  modifTime read before set is   : ", ctime(&statDatBefore.st_mtime) );
			SHOW_AND_LOG_ERROR2_NOENDL("  modifTime was set to          : ", ctime(&modifTime) );
			SHOW_AND_LOG_ERROR2_NOENDL("  modifTime read after set is   : ", ctime(&statDat.st_mtime) );
			return false;
		}


		#ifdef IS_WINDOWS
			// Windows only: Restore original attributes:
			if (!SetFileAttributes( fileName.c_str(), dwProp ))
			{
				SHOW_AND_LOG_ERROR2("ERROR: [changeFileTimes] Cannot restore original file attributes for file: ",fileName);
				return false;
			}
		#endif
		return true;
	}
	else
	{
		SHOW_AND_LOG_ERROR2("ERROR: [changeFileTimes] Cannot change file times for file: ",fileName);
		return false;
	}
}


#ifdef IS_WINDOWS

/* Compiling for Windows */

// ---------------------------------------------------------------
//  dirExplorer: Explores a given path and return the list
//     of its contents. Returns true if OK, false on any error.
// ---------------------------------------------------------------
bool dirExplorer( const std::string &inPath, TDirListing &outList )
{
	char			auxStr[33000];
	WIN32_FIND_DATA		f;
	TFileInfo		newEntry;

	outList.clear();
	sprintf(auxStr,"%s/*",inPath.c_str());

	HANDLE h = FindFirstFile( auxStr, &f);
	if(h == INVALID_HANDLE_VALUE)
	{
		SHOW_AND_LOG_ERROR2("CRITICAL ERROR: [dirExplorer] Cannot open directory: ",inPath);
		return false;
	}

	do
	{
		if ( strcmp( f.cFileName,".") && strcmp(f.cFileName,"..") )
		{
			// File name:
			newEntry.name = string(f.cFileName);

			// Complete file path:
			sprintf(auxStr,"%s/%s",inPath.c_str(),f.cFileName);
			newEntry.wholePath = string(auxStr);

			// File times:
			struct stat statDat;
			if (stat( newEntry.wholePath.c_str(),&statDat ))
			{
				SHOW_AND_LOG_ERROR2("CRITICAL ERROR: [dirExplorer] Cannot get file stats for: ",newEntry.wholePath);
				FindClose(h);
				return false;
			}

			newEntry.modTime    = statDat.st_mtime;
			newEntry.accessTime = statDat.st_atime;

			// Flags:
			newEntry.isDir = 0!=(statDat.st_mode &_S_IFDIR);
			newEntry.isSymLink = false; // (We donnot look for this in Windows, by now...)


			// Save:
			outList.push_back( newEntry );
		}
	} while(FindNextFile(h, &f));

	if (!FindClose(h))
	{
		showLastWindowsError();
		cerr << "ERROR: [dirExplorer] While closing FindClose: " << endl;
		return false;
	}

	return true; // OK
}

#else

/* Compiling for UNIX / POSIX */

#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>

// ---------------------------------------------------------------
//  dirExplorer: Explores a given path and return the list
//     of its contents. Returns true if OK, false on any error.
// ---------------------------------------------------------------
bool dirExplorer( const std::string &inPath, TDirListing &outList )
{
	char auxStr[33000];
	outList.clear();

	DIR *dir = opendir( inPath.c_str() );
	if (!dir)
	{
		SHOW_AND_LOG_ERROR2("CRITICAL ERROR: [dirExplorer] Cannot open directory: ",inPath);
		return false;
	}

	struct dirent *ent;
	TFileInfo       newEntry;

	while((ent = readdir(dir)) != NULL)
	{
		if ( strcmp(ent->d_name,".") && strcmp(ent->d_name,"..") )
		{
			// File name:
			newEntry.name = string(ent->d_name);

			// Complete file path:
			sprintf(auxStr,"%s/%s",inPath.c_str(),ent->d_name);
			newEntry.wholePath = string(auxStr);

			// File times:
			struct stat statDat, lstatDat;
			if (stat( newEntry.wholePath.c_str(),&statDat ))
			{
				// If it is a broken sym link, ignore the error and the file:
				if (! lstat( newEntry.wholePath.c_str(),&lstatDat ))
				{
					// Yes, it is a broken sym link: Ignore the continue with the next file:
					SHOW_AND_LOG_MSG2("[dirExplorer] WARNING: Ignoring broken link: ",newEntry.wholePath);
					continue;
				}
				else
				{
					// No, we really have a problem with this file:
					SHOW_AND_LOG_ERROR2("CRITICAL ERROR: [dirExplorer] Cannot get file stats for: ",newEntry.wholePath);
					closedir(dir);
					return false;
				}
			}

			newEntry.modTime    = statDat.st_mtime;
			newEntry.accessTime = statDat.st_atime;

			// Flags:
			newEntry.isDir = S_ISDIR(statDat.st_mode);

			// Is it a symbolic link?? Need to call "lstat":
			if (lstat( newEntry.wholePath.c_str(),&lstatDat ))
			{
				SHOW_AND_LOG_ERROR2("CRITICAL ERROR: [dirExplorer] Cannot get file lstat for: ",newEntry.wholePath);
				closedir(dir);
				return false;
			}
			newEntry.isSymLink = S_ISLNK(lstatDat.st_mode);

			// Save:
			outList.push_back( newEntry );
		}
	}

	closedir(dir);

	return true; // OK
}
#endif


/** Returns a string representation of a time ellapse
  */
string formatTime( long secs)
{
	// Decompose:
	long nYears = secs / (365*24*3600); secs %= (365*24*3600);
	long nDays = secs / (24*3600); secs %= (24*3600);
	long nHours = secs / (3600); secs %= (3600);
	long nMins = secs / 60; secs %= 60;

	string ret;
	char	aux[2000];
	if (nYears)
	{
		sprintf(aux,"%lu years,",nYears);
		ret = ret + string(aux);
	}
	if (nDays)
	{
		sprintf(aux,"%lu days,",nDays);
		ret = ret + string(aux);
	}
	if (nHours)
	{
		sprintf(aux,"%lu hours,",nHours);
		ret = ret + string(aux);
	}
	if (nMins)
	{
		sprintf(aux,"%lu mins,",nMins);
		ret = ret + string(aux);
	}
	sprintf(aux,"%lu secs",secs);
	ret = ret + string(aux);

	return ret;
}

// ---------------------------------------------------------------------
// Creates a new directory (does not fail if if already exists) 
//  In linux, the directory is created with RWX permisions for everyone 
//  Returns true on success, false on error.
// ---------------------------------------------------------------------
bool createDirectory( const std::string &path )
{
	if ( pathExists( path ) )	return true;

#ifdef IS_WINDOWS
	// Windows:
	return 0==_mkdir(path.c_str());
#else
	// Linux:
	return 0==mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IRWXO );
#endif
}

// ---------------------------------------------------------------------
//  copyDirectory - Recursively copy files and directories 
// 	   (the source & target MUST be directories - if target directory 
// 	   does not exist it will be created) 
//    This function preserves the original file timestamps
//	Returns true on success, false on error.
// ---------------------------------------------------------------------
bool copyDirectory( const std::string &org, const std::string &trg, long *copyCount  )
{
	// Check:
	if (!pathExists( org ))
	{
		SHOW_AND_LOG_ERROR2("ERROR: [copyDirectory] Source directory does not exist: ",org);
		return false;
	}

	// Create if target does not exist:
	if (!createDirectory( trg ))
	{
		SHOW_AND_LOG_ERROR2("ERROR: [copyDirectory] Cannot create target directory: ",trg);
		return false;
	}

	// Make a list with the contents of the source dir:
	TDirListing	filList;
	if (!dirExplorer( org, filList )) return false;

	// Copy the files/directories:
	for (TDirListing::iterator it=filList.begin(); it!=filList.end();it++)
	{
		if (!it->isDir)
		{
			// Copy the file:
			if (!copyFile(it->wholePath,trg+string("/")+string(it->name))) return false;
			if (copyCount) (*copyCount)++;
		}
		else
		{
			// Recursively copy the directories:
			if (!copyDirectory(it->wholePath,trg+string("/")+string(it->name),copyCount)) return false;
			if (copyCount) (*copyCount)++;
		}
	}

	// Copy the attributes of the source dir to the target dir (WIN only):
	if (!copyFileAttributes( org,trg ) ) return false;

	return true; // Done!
}

std::string rightPad(const std::string &str, const size_t total_len)
{
	std::string r = str;
	if (r.size()<total_len)
		r.resize(total_len,' ');
	return r;
}


std::string trim(const std::string &str)
{
	if (str.empty())
	{
		return std::string();
	}
	else
	{
		size_t s = str.find_first_not_of(" \t");
		size_t e = str.find_last_not_of(" \t");
		if (s==std::string::npos || e==std::string::npos)
				return std::string();
		else	return str.substr( s, e-s+1);
	}
}

string  lowerCase(const string& str)
{
	string outStr( str );

	transform(
		outStr.begin(), outStr.end(),		// In
		outStr.begin(),			// Out
		(int(*)(int)) tolower );
	return outStr;
}

string  upperCase(const string& str)
{
	string outStr( str );
	transform(
		outStr.begin(), outStr.end(),		// In
		outStr.begin(),			// Out
		(int(*)(int)) toupper );
	return outStr;
}

string convertBackslashes(const string &s) /// Convert \\ -> /
{
	string ret = s;
	replace_if(ret.begin(), ret.end(), std::bind2nd(std::equal_to<char>(),'\\'), '/');
	return ret;
}

/*---------------------------------------------------------------
						strtok
---------------------------------------------------------------*/
char *my_strtok( char *str, const char *strDelimit, char **context )
{
#if defined(_MSC_VER) && (_MSC_VER>=1400)
	// Use a secure version in Visual Studio 2005:
	return ::strtok_s(str,strDelimit,context);
#else
	// Use standard version:
	return ::strtok(str,strDelimit);
#endif
}

/*---------------------------------------------------------------
						tokenize
---------------------------------------------------------------*/
void  tokenize(
	const std::string			&inString,
	const std::string			&inDelimiters,
	std::vector<std::string>	&outTokens )
{
	char	*nextTok,*context;

	outTokens.clear();

	nextTok = my_strtok((char*)inString.c_str(),inDelimiters.c_str(),&context);
	while (nextTok != NULL)
	{
		outTokens.push_back( std::string(nextTok) );
		nextTok = my_strtok (NULL,inDelimiters.c_str(),&context);
	};
}

#include <cstdarg>

int my_vsnprintf(char *buf, size_t bufSize, const char *format, va_list args)
{
#if defined(_MSC_VER)
	#if (_MSC_VER>=1400)
		// Use a secure version in Visual Studio 2005:
		return ::vsnprintf_s (buf, bufSize, _TRUNCATE, format, args);
	#else
		return ::vsprintf(buf,format, args);
	#endif
#else
	// Use standard version:
	return ::vsnprintf(buf, bufSize,format, args);
#endif
}


string format(const char *fmt, ...)	//! A sprintf-like function for std::string 
{
	if (!fmt) return string("");

	int   result = -1, length = 1024;
	vector<char> buffer;
	while (result == -1)
	{
		buffer.resize(length + 10);

		va_list args;  // This must be done WITHIN the loop
		va_start(args,fmt);
		result = my_vsnprintf(&buffer[0], length, fmt, args);
		va_end(args);

		// Truncated?
		if (result>=length) result=-1;
		length*=2;
	}
	string s(&buffer[0]);
	return s;
}

string readTextFile(const string &filename)
{
	ifstream fi(filename.c_str());
	if (!fi.is_open()) return string();

	string buf;
	while (!fi.eof() && !fi.fail())
	{
		string s;
		getline(fi,s);
		buf+=s;
	}
	return buf;	
}



} // end namespace





