file(GLOB TEST_SOURCES "commands/*.cpp")
file(GLOB UTILS_SOURCES "utils/*.cpp")
add_executable(tests tests.cpp ${TEST_SOURCES} ${UTILS_SOURCES})
target_include_directories(tests PUBLIC ../ext ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(tests PUBLIC boost_libraries commands)

# Copy the test git repository
# Note: Since gitrepo is a submodule, we need to copy the actual .git directory
file(COPY ${CMAKE_SOURCE_DIR}/tests/gitrepo
     DESTINATION ${CMAKE_BINARY_DIR}/tests
     PATTERN ".git" EXCLUDE)

# Copy the submodule's .git directory as a real directory (not a gitlink)
if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/.git/modules/tests/gitrepo)
  file(COPY ${CMAKE_SOURCE_DIR}/.git/modules/tests/gitrepo/
       DESTINATION ${CMAKE_BINARY_DIR}/tests/gitrepo/.git)
endif()

include(../cmake/Catch.cmake)
catch_discover_tests(tests)
