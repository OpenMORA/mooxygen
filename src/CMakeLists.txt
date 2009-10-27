

# Declare the target (an executable)
ADD_EXECUTABLE(	mooxygen
	mooxygen_main.cpp
	)

TARGET_LINK_LIBRARIES(mooxygen  ${MRPT_LIBS})

INSTALL(TARGETS mooxygen
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib)
