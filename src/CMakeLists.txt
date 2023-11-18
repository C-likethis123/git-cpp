add_library(adder adder.cpp)
target_include_directories(adder PUBLIC ../include)
add_library(commands init.cpp)
target_include_directories(commands PUBLIC ../include)
# require at least c++11
target_compile_features(adder PUBLIC cxx_std_11)
target_compile_features(commands PUBLIC cxx_std_11)
