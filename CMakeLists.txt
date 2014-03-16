# ----------------------------------------------------------------------------
# Root CMake file for the Mooxygen project
#  Run with "cmake ." or "cmakegui" 
#
#  Jose Luis Blanco <jlblanco@ctima.uma.es>
#   http://code.google.com/p/mooxygen/
# ----------------------------------------------------------------------------

PROJECT(mooxygen)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# --------------------------------------------------------------
# Indicate CMake 2.7 and above that we don't want to mix relative
#  and absolute paths in linker lib lists.
# Run "cmake --help-policy CMP0003" for more information.
# --------------------------------------------------------------
if(COMMAND cmake_policy)
      cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)


# MOOXYGEN version from the .h file:
# -------------------------------------------
file(READ "src/moox_version.h" STR_MOOX_VERSION)

STRING(REGEX MATCHALL "[0-9]+.[0-9]+.[0-9]+" MOOXYGEN_VERSION "${STR_MOOX_VERSION}")

STRING(REGEX MATCHALL "[0-9]+" MOOXYGEN_VERSION_PARTS "${MOOXYGEN_VERSION}")
LIST(GET MOOXYGEN_VERSION_PARTS 0  MOOXYGEN_VERSION_MAJOR)
LIST(GET MOOXYGEN_VERSION_PARTS 1  MOOXYGEN_VERSION_MINOR)
LIST(GET MOOXYGEN_VERSION_PARTS 2  MOOXYGEN_VERSION_PATCH)

MESSAGE(STATUS "Parsed Mooxygen version: ${MOOXYGEN_VERSION_MAJOR}.${MOOXYGEN_VERSION_MINOR}.${MOOXYGEN_VERSION_PATCH}")




SET( EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/ CACHE PATH "Output directory for applications" )


# Go and define the targets
add_subdirectory(src)


# ----------------------------------------------------------------------------
# Include the "CPack" package generator
# ----------------------------------------------------------------------------
INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Mooxygen - A documentation generator for MOOS")
SET(CPACK_PACKAGE_VENDOR "Jose Luis Blanco Claraco")
SET(CPACK_PACKAGE_CONTACT "Jose Luis Blanco Claraco <jlblanco@ctima.uma.es>")

SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
SET(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

SET(CPACK_PACKAGE_VERSION_MAJOR "${MOOXYGEN_VERSION_MAJOR}")
SET(CPACK_PACKAGE_VERSION_MINOR "${MOOXYGEN_VERSION_MINOR}")
SET(CPACK_PACKAGE_VERSION_PATCH "${MOOXYGEN_VERSION_PATCH}")

SET(CPACK_SOURCE_GENERATOR "TGZ;ZIP")

SET(CPACK_PACKAGE_INSTALL_DIRECTORY "mooxygen-${MOOXYGEN_VERSION_MAJOR}.${MOOXYGEN_VERSION_MINOR}.${MOOXYGEN_VERSION_PATCH}")

IF(WIN32)
	# --------------------------------
	# Packages for Windows
	# --------------------------------
	SET(CPACK_SOURCE_IGNORE_FILES ".svn/;.*~;build;CMakeCache.txt;_CPack_Pakages/;CMakeFiles/;install/;Makefile;*.cmake")

	SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\http://code.google.com/p/mooxygen/")
	SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.isa.uma.es\\\\jlblanco")
	SET(CPACK_NSIS_CONTACT "joseluisblancoc@gmail.com")
	
	# Add mrpt/bin dir to system PATH
	SET(CPACK_NSIS_MODIFY_PATH ON)

	# Install header and source files:
	# ---------------------------------------------
	INSTALL(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/"
		DESTINATION src
		PATTERN ".svn" EXCLUDE
		PATTERN "*~" EXCLUDE)

	INSTALL(FILES 
		CMakeLists.txt
		COPYING
		README.txt
	DESTINATION .)
		
	SET(CPACK_PACKAGE_EXECUTABLES 
		"mooxygen;Mooxygen")
		
	SET(CPACK_NSIS_MENU_LINKS
	    "bin;Mooxygen BIN directory")
	

ENDIF(WIN32)
IF(UNIX)
	SET(CPACK_SOURCE_IGNORE_FILES ".svn;CMakeFiles;CMakeCache.txt;CPack*;progress.make;install_manifest.txt;cmake_install.cmake;cmake_psdk.cmake$;Makefile;.swp$;.*~;tags;cmake_uninstall.cmake$")
   	SET(CPACK_OUTPUT_FILE_PATH "mooxygen-${MOOXYGEN_VERSION_MAJOR}.${MOOXYGEN_VERSION_MINOR}.${MOOXYGEN_VERSION_PATCH}.deb")
	SET(CPACK_GENERATOR "TGZ;DEB")
ENDIF(UNIX)

IF(APPLE)
	SET(CPACK_GENERATOR "TGZ;TBZ2;OSXX11")
Endif(APPLE)

INCLUDE(CPack)

