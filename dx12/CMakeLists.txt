cmake_minimum_required(VERSION 3.29)

project(VertexPipelines
	VERSION 0.1
	LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

cmake_path(SET DIRECTX_HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../DirectX-Headers/include/directx)

add_compile_definitions(_UNICODE UNICODE)

add_executable(dx12
	WIN32
	main.cpp
	DXWindow.h
	DXWindow.cpp
	Frame.h
	Frame.cpp)

target_precompile_headers(dx12 PRIVATE stdafx.h) 

target_include_directories(dx12 PRIVATE ${DIRECTX_HEADER_PATH})

add_subdirectory(windows)
target_link_libraries(dx12 PRIVATE windows)
target_include_directories(dx12 PRIVATE ${CMAKE_CURRENT_LIST_DIR}/windows)

add_subdirectory(dxutil)
target_link_libraries(dx12 PRIVATE dxutil)
target_include_directories(dx12 PRIVATE ${CMAKE_CURRENT_LIST_DIR}/dxutil)

target_link_libraries(dx12 PRIVATE d3d12.lib dxgi.lib)
