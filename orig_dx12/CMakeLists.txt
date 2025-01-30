cmake_minimum_required(VERSION 3.29)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(_UNICODE UNICODE)

project(dxultra LANGUAGES CXX)

cmake_path(SET DIRECTX_HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../DirectX-Headers/include/directx)

add_subdirectory(common)
add_subdirectory(windows)
add_subdirectory(dxutil)

add_executable(dxultra WIN32 "main.cpp" "DXUltra.cpp" "DXUltra.h")

add_subdirectory(shaders)

target_include_directories(dxultra PRIVATE ${DIRECTX_HEADER_PATH})

target_link_libraries(dxultra common)
target_link_libraries(dxultra windows)
target_link_libraries(dxultra dxutil)

target_link_libraries(dxultra d3d12.lib dxgi.lib D3DCompiler.lib)
