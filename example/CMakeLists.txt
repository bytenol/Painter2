project(painter2_example)

add_executable(window window.cpp)

target_link_libraries(window PRIVATE painter2 glfw)