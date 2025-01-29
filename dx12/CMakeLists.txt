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
	main.cpp
	DXWindow.h
	DXWindow.cpp
	Error.h
	DescriptorHeap.h
	DescriptorHeap.cpp
	SwapChain.h
	SwapChain.cpp
	Frame.h
	Frame.cpp
	Constants.h
	Fence.h
	Fence.cpp)

target_include_directories(dx12
	PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/../DirectX-Headers/include/directx)

target_precompile_headers(dx12
	PRIVATE stdafx.h)

add_subdirectory(windows)
include_directories(windows)

target_link_libraries(dx12 windows)

target_link_libraries(dx12 d3d12.lib dxgi.lib)
