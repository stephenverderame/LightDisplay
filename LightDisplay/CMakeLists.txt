# CMakeList.txt : CMake project for LightDisplay, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

find_package(OpenGL REQUIRED)

# Setup glad
add_subdirectory ("glad")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source to this project's executable.
file(GLOB_RECURSE HEADERS "${CMAKE_SOURCE_DIR}/LightDisplay/include/*.h")
file(GLOB_RECURSE CPPS "${CMAKE_SOURCE_DIR}/LightDisplay/src/*.cpp")
set(INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/LightDisplay/include")
set(SOURCES ${CPPS} ${HEADERS})

# Create executable
add_executable (LightDisplay ${SOURCES} "LightDisplay.cpp")
target_include_directories(LightDisplay PRIVATE ${INCLUDE_DIRECTORIES})
add_dependencies(LightDisplay OpenGL::GL glfw glad glm)
target_link_libraries(LightDisplay PRIVATE glfw PRIVATE OpenGL::GL PRIVATE glad PRIVATE glm)
if(WIN32)
	target_link_libraries(LightDisplay PRIVATE wsock32 PRIVATE ws2_32 PRIVATE Bthprops)
endif()


function(cp_dir source dest)
	make_directory(${dest})
	file(GLOB source_files RELATIVE "${source}" "${source}/*") #Files are specified relative to ${source}
	foreach(templateFile ${source_files})
		set(src_path ${source}/${templateFile})
		message(${src_path})
		if(NOT IS_DIRECTORY ${src_path})
			configure_file(${src_path} ${dest}/${templateFile} @ONLY)
		else()
			cp_dir(${src_path} ${dest}/${templateFile})
		endif()
	endforeach()
endfunction()

cp_dir("${CMAKE_CURRENT_SOURCE_DIR}/scripts" "${CMAKE_CURRENT_BINARY_DIR}/scripts")
cp_dir("${CMAKE_CURRENT_SOURCE_DIR}/scripts" "${CMAKE_CURRENT_BINARY_DIR}/test/scripts") #debug directory

# Add tests
add_subdirectory("test")