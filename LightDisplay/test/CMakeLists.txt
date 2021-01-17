
cmake_minimum_required(VERSION 3.8)
function(make_test NAME)
	set(BOOLEAN_ARGS "")
	set(ONEVALUE_ARGS "WORKING_DIRECTORY" "INCLUDE_DIR")
	set(MULTIVALUE_ARGS "SOURCES")
	cmake_parse_arguments(
		MK_TEST
		"${BOOLEAN_ARGS}"
		"${ONEVALUE_ARGS}"
		"${MULTIVALUE_ARGS}"
		${ARGN}
	)

	add_executable(${NAME} ${MK_TEST_SOURCES} ${SOURCES})

	if(${MK_TEST_INCLUDE_DIR})
		target_include_directories(${NAME} PRIVATE ${INCLUDE_DIRECTORIES} PRIVATE ${MK_TEST_INCLUDE_DIR})
	else()
		target_include_directories(${NAME} PRIVATE ${INCLUDE_DIRECTORIES})
	endif()

	add_dependencies(${NAME} gtest gmock)
	target_link_libraries(${NAME} PRIVATE gtest PRIVATE gmock PRIVATE glfw 
		PRIVATE OpenGL::GL PRIVATE glad PRIVATE glm)

	if(${MK_TEST_WORKING_DIRECTORY})
		add_test(NAME ${NAME} COMMAND ${NAME}
			WORKING_DIRECTORY ${MK_TEST_WORKING_DIRECTORY})
	else()
		add_test(NAME ${NAME} COMMAND ${NAME})
	endif()

endfunction()

make_test(FFTTest SOURCES "FFTTest.cpp" "FFTCmpImpl.h")
make_test(AudioTest SOURCES "AudioSourceTest.cpp" 
	WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/LightDisplay/test/res")
cp_dir("${CMAKE_CURRENT_SOURCE_DIR}/res" "${CMAKE_CURRENT_BINARY_DIR}/res")
