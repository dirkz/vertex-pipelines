add_library(windows OBJECT "Window.cpp" "Window.h" "Strings.cpp" "Strings.h")

target_include_directories(windows PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(windows common)
