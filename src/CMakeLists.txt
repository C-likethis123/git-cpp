add_library(adder adder.cpp)
target_include_directories(adder PUBLIC ../include)
target_compile_features(adder PUBLIC cxx_std_11)

add_library(commands init.cpp)
target_include_directories(commands PUBLIC ../include)
target_compile_features(commands PUBLIC cxx_std_11)

add_library(repository repository.cpp)
target_include_directories(repository PUBLIC ../include)
target_compile_features(repository PUBLIC cxx_std_11)

add_library(inih inih.cpp)
target_include_directories(inih PUBLIC ../include)
