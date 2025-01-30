cmake_minimum_required(VERSION 3.29)

project(VertexPipelines
	VERSION 0.1
	LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_definitions(_UNICODE UNICODE)

cmake_path(SET DIRECTX_HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../DirectX-Headers/include/directx)

add_subdirectory(common)
add_subdirectory(windows)
add_subdirectory(dxutil)

add_subdirectory(shaders)

add_executable(dx12
	WIN32
	main.cpp
	DXWindow.h
	DXWindow.cpp
	Frame.h
	Frame.cpp "Vertex.h" "Vertex.cpp")

target_include_directories(dx12 PRIVATE ${DIRECTX_HEADER_PATH})

target_link_libraries(dx12 PRIVATE common)
target_link_libraries(dx12 PRIVATE windows)
target_link_libraries(dx12 PRIVATE dxutil)

add_dependencies(dx12 shaders)

target_link_libraries(dx12 PRIVATE d3d12.lib dxgi.lib dxguid.lib D3DCompiler.lib)
