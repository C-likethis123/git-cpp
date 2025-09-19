cmake_minimum_required(VERSION 3.16)

# Set a name and a version number for your project:
project(
  cpp-project-template
  VERSION 0.0.1
  LANGUAGES CXX)

set(CMAKE_AR /usr/bin/ar)

# Enable C language
enable_language(C)

# sets OS version
set(OS_VERSION "" CACHE STRING "Version of the build")

message(STATUS "Building on: ${OS_VERSION}")

# Set flags based on OS_VERSION
if(OS_VERSION STREQUAL "1")
    set(HAVE_ARPA_INET_H 1)
    set(LIBS_TO_LINK lib_mac)
elseif(OS_VERSION STREQUAL "2")
    set(HAVE_WINSOCK2_H 1)
    set(LIBS_TO_LINK lib_win)
elseif(OS_VERSION STREQUAL "3")
    set(HAVE_ARPA_INET_H 1)
    set(LIBS_TO_LINK lib_linux)
else()
    message(FATAL_ERROR "Unknown OS_VERSION: ${OS_VERSION}. Must be 1,2 or 3.")
endif()

# Generate config.h
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.h
)

# Make the generated config.h available globally
include_directories(${CMAKE_CURRENT_BINARY_DIR})


# this needs to be in the top level CMakeLists.txt to enable tests
include(CTest)

set(BOOST_ENABLE_CMAKE ON)
set(Boost_CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BOOST_INCLUDE_LIBRARIES iostreams uuid algorithm)

include(FetchContent)
FetchContent_Declare(
  Boost
  GIT_REPOSITORY https://github.com/boostorg/boost.git
  GIT_TAG boost-1.85.0
  GIT_SHALLOW TRUE)
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
  doxygen_add_docs(doxygen ${CMAKE_SOURCE_DIR}
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endif()

# compile the tests
option(BUILD_TESTS "Enable tests" OFF)
if(BUILD_TESTS)
  add_subdirectory(tests)
endif()
