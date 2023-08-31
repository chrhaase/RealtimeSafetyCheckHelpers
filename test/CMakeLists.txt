enable_testing()

add_executable(Test main.cpp)
target_link_libraries(Test PRIVATE ${PROJECT_NAME})

add_test(NAME Tests COMMAND $<TARGET_FILE:Test>)
