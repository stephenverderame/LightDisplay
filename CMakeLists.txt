# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

macro(external_add filename pkname)
	configure_file(${filename} "${pkname}-download/CMakeLists.txt")
	execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" .
		WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/${pkname}-download")
	execute_process(COMMAND "${CMAKE_COMMAND}" --build .
		WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/${pkname}-download")
	set(${pkname}_SUBDIRS "${CMAKE_BINARY_DIR}/${pkname}-src"
	"${CMAKE_BINARY_DIR}/${pkname}-build")
endmacro() 

#Install googletest
external_add(CMakeLists.txt.in gtest)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

add_subdirectory(${gtest_SUBDIRS})

#Install glfw
external_add(CMakeListsGlfw.txt.in glfw)

#Disable GLFW test and example builds
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(GLFW_INCLUDE_DIR "${CMAKE_BINARY_DIR}/glfw-src/include")
set(GLFW_SOURCE_DIR "${CMAKE_BINARY_DIR}/glfw-src/src")

if(WIN32)
	set(_GLFW_WIN32)
endif()

add_subdirectory(${glfw_SUBDIRS})

#GLM
external_add(CMakeListsGlm.txt.in glm)
add_subdirectory(${glm_SUBDIRS})

enable_testing()

project ("LightDisplay")

include (CTest)

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	add_definitions("-W4" "-WX")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GCC")
	add_definitions("-Wall" "-Werror")
endif()

# Include sub-projects.
add_subdirectory ("LightDisplay")
