cmake_minimum_required(VERSION 3.8.2)

# Set a name and a version number for your project:
project(cpp-project-template VERSION 0.0.1 LANGUAGES CXX)

# Enable C language
enable_language(C)

# this needs to be in the top level CMakeLists.txt to enable tests
include(CTest)

set(BOOST_ENABLE_CMAKE ON)
set(Boost_CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
  Boost
  GIT_REPOSITORY https://github.com/boostorg/boost.git
  GIT_TAG boost-1.83.0
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(Boost)

# compile the library
add_subdirectory(src)

# compile the application
add_subdirectory(app)

# optionally add doxygen target to generate documentation
option(BUILD_DOCS "Enable building of documentation (requires Doxygen)" OFF)
if(BUILD_DOCS)
	find_package(Doxygen REQUIRED)
	set(DOXYGEN_EXCLUDE_PATTERNS "${CMAKE_SOURCE_DIR}/ext/*")
	doxygen_add_docs(doxygen
		${CMAKE_SOURCE_DIR}
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	)
endif()

# compile the tests
option(BUILD_TESTS "Enable building of documentation (requires Doxygen)" OFF)
if(BUILD_TESTS)
	add_subdirectory(tests)
endif()
