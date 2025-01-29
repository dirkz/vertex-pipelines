add_library(dxutil
	OBJECT
	DescriptorHeap.cpp
	Fence.cpp
	SwapChain.cpp)

target_sources(dxutil PUBLIC Constants.h Error.h DescriptorHeap.h Fence.h SwapChain.h)

target_include_directories(dxutil
	PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/../../DirectX-Headers/include/directx)

target_precompile_headers(dxutil
	PRIVATE stdafx.h)
