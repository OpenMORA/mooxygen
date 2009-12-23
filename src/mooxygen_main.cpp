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

// ------------------------------------------------------
void createTemplateFile();
void showUsage(const char *cmdLine);

// ------------------------------------------------------
//                        MAIN
// ------------------------------------------------------
int main(int argc, char**argv)
{
	try {
		/****************************
            0. Process command line
		 ****************************/
		if (argc>2)
		{
			showUsage(argv[0]);
			return -1;
		}

		if (argc==2 && argv[1][0]=='-')
		{
			if (!strcmp(argv[1],"--create"))
			{
				createTemplateFile();
				return 0;
			}
			cerr << "ERROR: Unknown command: " << argv[1] << endl;
			showUsage(argv[0]);
			return -1;
		}

		string  mooxyfilename = "./Mooxyfile";
		if (argc==2)
			mooxyfilename = argv[1];

		/****************************
            1. Load config
		 ****************************/
		TApplication  project;
		
		if (!project.opts.loadFromFile(mooxyfilename))
			return -1;

		/****************************
            2. Get list of files
		 ****************************/
		cout << "Scanning for source files..." << endl;
		if (!project.scanForSourceFiles()) 
			return -1;

		cout << project.lstSourceFiles.size() << " source files found." << endl;

		/****************************
            3. Analize source code
		 ****************************/
		cout << "Analyzing source files..." << endl;
		if (!project.parseSourceFiles()) 
			return -1;


		/****************************
            4. Generate outputs
		 ****************************/

		


		// All done!
		cout << "Mooxygen is done." << endl;
		return 0;
	} catch (exception &e) {
		cerr << "Program finished due to error: " << e.what() << endl;
		return -1;
	}
}


// ------------------------------------------------------
//                    EXIT
// ------------------------------------------------------
void mooxygen::exitProgram(int errCode, bool waitKey)
{
	if (waitKey) waitForKey();
	exit(errCode);
}


// ------------------------------------------------------
// ------------------------------------------------------
void showUsage(const char *cmdLine)
{
	cout << " Usage: " << cmdLine << " [config_file]" << endl;
	cout << "    or: " << cmdLine << " --create" << endl;
	cout << "  If invoked without arguments, the configuration file will be assumed to be \"Mooxyfile\"" << endl;
	cout << "  On a --create command, a Mooxyfile file will be created with a template project file." << endl;
	cout << "  For more help:  http://code.google.com/p/mooxygen/" << endl;
}

// ------------------------------------------------------
// ------------------------------------------------------
void createTemplateFile()
{
	string out_fil = "./Mooxyfile";

	cout << "Generating template file: " << out_fil << endl;

	CMooxygenOptions dummy;
	if (dummy.saveTemplateFile(out_fil))
		cout << "Generation OK." << endl;
}

