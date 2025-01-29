add_library(windows
	OBJECT
	Window.cpp)

target_sources(windows PUBLIC Window.h WindowCallback.h)

target_precompile_headers(windows
	PRIVATE stdafx.h)
