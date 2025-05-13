add_executable(tests tests.cpp repository_t.cpp tag_t.cpp)
target_include_directories(tests PUBLIC ../ext)

target_link_libraries(tests PUBLIC boost_libraries repository commands)
file(COPY ${CMAKE_SOURCE_DIR}/tests/gitrepo 
     DESTINATION ${CMAKE_BINARY_DIR}/tests
     FILES_MATCHING PATTERN ".*" PATTERN "*" EXCLUDE)

# allow user to run tests with `make test` or `ctest`
include(../cmake/Catch.cmake)
catch_discover_tests(tests)