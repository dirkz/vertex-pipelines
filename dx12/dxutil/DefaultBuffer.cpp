#include "DefaultBuffer.h"

#include "Error.h"

namespace zdx
{

DefaultBuffer::DefaultBuffer(ID3D12Device *pDevice, ID3D12GraphicsCommandList *pCommandList,
                             UINT64 size, const void *pData)
{
    CD3DX12_HEAP_PROPERTIES heapPropertiesDefault{D3D12_HEAP_TYPE_DEFAULT};
    CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(size);
    ThrowIfFailed(pDevice->CreateCommittedResource(&heapPropertiesDefault, D3D12_HEAP_FLAG_NONE,
                                                   &desc, D3D12_RESOURCE_STATE_COMMON, nullptr,
                                                   IID_PPV_ARGS(m_defaultBuffer.GetAddressOf())));

    CD3DX12_HEAP_PROPERTIES heapPropertiesUpload{D3D12_HEAP_TYPE_UPLOAD};
    ThrowIfFailed(pDevice->CreateCommittedResource(
        &heapPropertiesUpload, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr, IID_PPV_ARGS(m_uploadBuffer.GetAddressOf())));

    D3D12_SUBRESOURCE_DATA subResourceData{};
    subResourceData.pData = pData;
    subResourceData.RowPitch = size;
    subResourceData.SlicePitch = size;

    auto transition1 = CD3DX12_RESOURCE_BARRIER::Transition(
        m_defaultBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
    pCommandList->ResourceBarrier(1, &transition1);

    UpdateSubresources<1>(pCommandList, m_defaultBuffer.Get(), m_uploadBuffer.Get(), 0, 0, 1,
                          &subResourceData);

    auto transition2 = CD3DX12_RESOURCE_BARRIER::Transition(
        m_defaultBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COMMON);
    pCommandList->ResourceBarrier(1, &transition2);
}

ComPtr<ID3D12Resource1> DefaultBuffer::Buffer()
{
    return m_defaultBuffer;
}

} // namespace zdx
