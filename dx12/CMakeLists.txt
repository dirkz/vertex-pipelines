cmake_minimum_required(VERSION 3.29)

project(VertexPipelines
	VERSION 0.1
	LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(dx12
	WIN32
	main.cpp)

add_compile_definitions(_UNICODE UNICODE)

target_precompile_headers(dx12
	PRIVATE stdafx.h)
