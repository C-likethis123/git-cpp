# Create an interface target to group Boost libraries
add_library(boost_libraries INTERFACE)
target_link_libraries(boost_libraries INTERFACE Boost::filesystem Boost::system Boost::iostreams Boost::uuid Boost::format Boost::algorithm)

add_library(util util.cpp)
target_link_libraries(util PRIVATE boost_libraries)
target_include_directories(util PUBLIC ../include)
target_compile_features(util PUBLIC cxx_std_17)

add_library(inih inih.c inih.cpp)
target_include_directories(inih PUBLIC ../include)

add_library(repository repository.cpp)
target_link_libraries(repository PRIVATE inih util boost_libraries)
target_include_directories(repository PUBLIC ../include)
target_compile_features(repository PUBLIC cxx_std_17)

add_library(object object.cpp blob.cpp commit.cpp)
target_link_libraries(object PRIVATE repository boost_libraries)
target_include_directories(object PUBLIC ../include)
target_compile_features(object PUBLIC cxx_std_17)

add_library(commands init.cpp cat-file.cpp hash-object.cpp log.cpp)
target_link_libraries(commands PRIVATE repository object boost_libraries)
target_include_directories(commands PUBLIC ../include)
target_compile_features(commands PUBLIC cxx_std_17)
