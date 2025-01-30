add_library(dxutil
	OBJECT
	DescriptorHeap.h
	DescriptorHeap.cpp
	Fence.h
	Fence.cpp
	SwapChain.h
	SwapChain.cpp)

target_include_directories(dxutil PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(dxutil PRIVATE common)
