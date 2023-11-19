add_executable(adder_app adder_app.cpp)
target_link_libraries(adder_app PRIVATE ${Boost_LIBRARIES})
target_link_libraries(adder_app PRIVATE commands)
