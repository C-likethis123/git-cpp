# Create an interface target to group Boost libraries
add_library(boost_libraries INTERFACE)
target_link_libraries(
  boost_libraries
  INTERFACE Boost::iostreams
            Boost::uuid
            Boost::algorithm)

add_library(pack_index pack_index.cpp)
target_include_directories(pack_index PUBLIC ../include)
target_compile_features(pack_index PUBLIC cxx_std_17)

add_library(util util.cpp repo_util.cpp pack_util.cpp)
target_link_libraries(util PRIVATE boost_libraries)
target_include_directories(util PUBLIC ../include)
target_compile_features(util PUBLIC cxx_std_17)

add_library(inih inih.c inih.cpp)
target_include_directories(inih PUBLIC ../include)

file(GLOB WILDMATCH_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/wildmatch/wildmatch.cpp")
add_library(wildmatch ${WILDMATCH_SOURCES})
target_include_directories(wildmatch PUBLIC ../include)

add_library(repository repository.cpp)
target_link_libraries(repository PRIVATE inih util pack_index boost_libraries wildmatch)
target_include_directories(repository PUBLIC ../include)
target_compile_features(repository PUBLIC cxx_std_17)

add_library(object object.cpp blob.cpp commit.cpp tree.cpp index.cpp index_entry.cpp)
target_link_libraries(object PRIVATE util repository boost_libraries)
target_include_directories(object PUBLIC ../include)
target_compile_features(object PUBLIC cxx_std_17)

file(GLOB PARSER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/parsers/*.cpp")
add_library(parsers ${PARSER_SOURCES})
target_link_libraries(parsers PRIVATE repository object boost_libraries)
target_include_directories(parsers PUBLIC ../include)
target_compile_features(parsers PUBLIC cxx_std_17)

file(GLOB COMMANDS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/commands/*.cpp")
add_library(commands ${COMMANDS_SOURCES})
target_link_libraries(commands PRIVATE parsers util repository object boost_libraries)
target_include_directories(commands PUBLIC ../include)
target_compile_features(commands PUBLIC cxx_std_17)
