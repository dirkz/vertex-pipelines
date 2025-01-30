add_library(dxutil OBJECT
	"Fence.cpp"
	"Fence.h"
	"SwapChain.cpp"
	"SwapChain.h"
	"Frame.cpp"
	"Frame.h"
	"DefaultBufferUploader.cpp"
	"DefaultBufferUploader.h"
	"CommandList.cpp"
	"CommandList.h"
	"Shader.cpp"
	"Shader.h"
	"UploadBuffer.h"
	"VertexPipeline.cpp"
	"VertexPipeline.h"
	"DrawData.cpp"
	"DrawData.h"
	"Geometry.cpp"
	"Geometry.h"
	"IndexedVertexBuffer.h"
	"Vertex.cpp"
	"Vertex.h"
	"Mesh.h"
	"MeshEntry.cpp"
	"MeshEntry.h")

target_include_directories(dxutil PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(dxutil common dxguid.lib)