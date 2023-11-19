add_library(util util.cpp)
target_include_directories(util PUBLIC ../include)
target_compile_features(util PUBLIC cxx_std_11)

add_library(inih inih.cpp)
target_include_directories(inih PUBLIC ../include)

add_library(repository repository.cpp)
target_link_libraries(repository PUBLIC inih util)
target_include_directories(repository PUBLIC ../include)
target_compile_features(repository PUBLIC cxx_std_11)

add_library(commands init.cpp)
target_link_libraries(commands PRIVATE repository)
target_include_directories(commands PUBLIC ../include)
target_compile_features(commands PUBLIC cxx_std_11)
