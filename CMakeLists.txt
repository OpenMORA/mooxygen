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

SET( EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/ CACHE PATH "Output directory for applications" )


# Go and define the targets
add_subdirectory(src)
