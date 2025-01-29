add_library(windows
	OBJECT
	WindowCallback.h
	Window.h
	Window.cpp)

target_precompile_headers(windows
	PRIVATE stdafx.h)
