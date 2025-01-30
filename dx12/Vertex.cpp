#include "Vertex.h"

namespace zdx
{

D3D12_INPUT_LAYOUT_DESC Vertex::InputLayout()
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
                                 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, InstanceDataStepRate}};

    return D3D12_INPUT_LAYOUT_DESC{elements, _countof(elements)};
}

} // namespace zdx
