#include "VertexPipeline.h"

#include "Vertex.h"

namespace dxultra
{

VertexPipeline::VertexPipeline(ID3D12Device *pDevice)
    : m_vertexShader{L"shaders\\basic_triangle.hlsl_VS.cso", "VS", "vs_5_1"},
      m_pixelShader{L"shaders\\basic_triangle.hlsl_PS.cso", "PS", "ps_5_1"}
{
}

ID3DBlob *VertexPipeline::VertexShader()
{
    return m_vertexShader.Code().Get();
}

ID3DBlob *VertexPipeline::PixelShader()
{
    return m_pixelShader.Code().Get();
}

D3D12_INPUT_LAYOUT_DESC VertexPipeline::InputLayout()
{
    constexpr UINT InputSlot = 0;
    constexpr UINT SemanticIndex = 0;
    constexpr UINT InstanceDataStepRate = 0;

    static constexpr D3D12_INPUT_ELEMENT_DESC elements[] = {
        D3D12_INPUT_ELEMENT_DESC{"POSITION", SemanticIndex, DXGI_FORMAT_R32G32B32_FLOAT, InputSlot,
                                 offsetof(Vertex, Position),
                                 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, InstanceDataStepRate},
        D3D12_INPUT_ELEMENT_DESC{"COLOR", SemanticIndex, DXGI_FORMAT_R32G32B32A32_FLOAT, InputSlot,
                                 offsetof(Vertex, Color),
                                 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, InstanceDataStepRate},
        D3D12_INPUT_ELEMENT_DESC{"TEXCOORD", SemanticIndex, DXGI_FORMAT_R32G32_FLOAT, InputSlot,
                                 offsetof(Vertex, TexCoord),
                                 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, InstanceDataStepRate}};

    return D3D12_INPUT_LAYOUT_DESC{elements, _countof(elements)};
}

} // namespace dxultra