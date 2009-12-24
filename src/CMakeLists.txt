

# Declare the target (an executable)
ADD_EXECUTABLE(	mooxygen
	moox_version.h
	mooxygen_main.cpp
	moox_utils.cpp moox_utils.h
	moox_app.cpp moox_app.h
	moox_app_gen_html.cpp
	CMooxygenOptions.cpp CMooxygenOptions.h		
	moox_resources.cpp
	)

#TARGET_LINK_LIBRARIES(mooxygen  ${MRPT_LIBS})

INSTALL(TARGETS mooxygen
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib)
