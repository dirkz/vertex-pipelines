cmake_minimum_required(VERSION 3.29)

project(VertexPipelines
	VERSION 0.1
	LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_definitions(_UNICODE UNICODE)

add_executable(dx12
	WIN32
	WindowCallback.h
	Window.h
	Window.cpp
	main.cpp
	"WindowCallback.h"
	"WindowCallback.h"
	"DX12Window.h"
	"DX12Window.cpp"
	"Error.h")

target_include_directories(dx12
	PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/../DirectX-Headers/include/directx)

target_precompile_headers(dx12
	PRIVATE stdafx.h)

target_link_libraries(dx12 d3d12.lib)
