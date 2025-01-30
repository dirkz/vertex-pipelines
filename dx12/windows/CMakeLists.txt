add_library(windows
	OBJECT
	Window.h
	WindowCallback.h
	Window.cpp)

target_include_directories(windows PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(windows PRIVATE common)
