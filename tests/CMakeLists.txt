file(GLOB TEST_SOURCES "commands/*.cpp")
add_executable(tests tests.cpp ${TEST_SOURCES} utils/gitreposetup.cpp)
target_include_directories(tests PUBLIC ../ext ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(tests PUBLIC boost_libraries commands)
file(COPY ${CMAKE_SOURCE_DIR}/tests/gitrepo
     DESTINATION ${CMAKE_BINARY_DIR}/tests)

include(../cmake/Catch.cmake)
catch_discover_tests(tests)
