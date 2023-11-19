add_executable(tests tests.cpp repository_t.cpp)
target_include_directories(tests PUBLIC ../ext)

target_link_libraries(tests PUBLIC repository)

# allow user to run tests with `make test` or `ctest`
include(../cmake/Catch.cmake)
catch_discover_tests(tests)