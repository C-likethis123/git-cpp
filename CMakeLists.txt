cmake_minimum_required(VERSION 3.24)

# Set a name and a version number for your project:
project(
  cpp-project-template
  VERSION 0.0.1
  LANGUAGES CXX)

set(CMAKE_AR /usr/bin/ar)

# Enable C language
enable_language(C)


# this needs to be in the top level CMakeLists.txt to enable tests
include(CTest)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
FetchContent_Declare(
  vog_sha1
  GIT_REPOSITORY https://github.com/vog/sha1.git
  GIT_TAG 3f8a4aa032d144309d00dbfe972906a49b3631b9)
FetchContent_MakeAvailable(vog_sha1)
find_package(ZLIB REQUIRED)
FetchContent_Declare(
  zstr
  GIT_REPOSITORY https://github.com/mateidavid/zstr.git
  GIT_TAG dc86d3d9e516a6ed44d4a4049faf73e5fc0e1dde) # v1.1.0
FetchContent_MakeAvailable(zstr)

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
