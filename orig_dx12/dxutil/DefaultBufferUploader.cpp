#include "DefaultBufferUploader.h"

namespace dxultra
{

DefaultBufferUploader::DefaultBufferUploader(ComPtr<ID3D12Device> device)
    : m_device{device}, m_commandList{device.Get()}
{
}

ComPtr<ID3D12Resource1> DefaultBufferUploader::CreateDefaultBuffer(
    const void *pData, UINT64 size, D3D12_RESOURCE_STATES targetResourceState)
{
    ComPtr<ID3D12Resource1> defaultBuffer;
    CD3DX12_HEAP_PROPERTIES heapPropertiesDefault{D3D12_HEAP_TYPE_DEFAULT};
    CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(size);
    ThrowIfFailed(m_device->CreateCommittedResource(&heapPropertiesDefault, D3D12_HEAP_FLAG_NONE,
                                                    &desc, D3D12_RESOURCE_STATE_COMMON, nullptr,
                                                    IID_PPV_ARGS(defaultBuffer.GetAddressOf())));

    ComPtr<ID3D12Resource1> uploadBuffer;
    CD3DX12_HEAP_PROPERTIES heapPropertiesUpload{D3D12_HEAP_TYPE_UPLOAD};
    ThrowIfFailed(m_device->CreateCommittedResource(
        &heapPropertiesUpload, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr, IID_PPV_ARGS(uploadBuffer.GetAddressOf())));

    D3D12_SUBRESOURCE_DATA subResourceData{};
    subResourceData.pData = pData;
    subResourceData.RowPitch = size;
    subResourceData.SlicePitch = size;

    auto transition1 = CD3DX12_RESOURCE_BARRIER::Transition(
        defaultBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
    m_commandList->ResourceBarrier(1, &transition1);

    UpdateSubresources<1>(m_commandList.Get(), defaultBuffer.Get(), uploadBuffer.Get(), 0, 0, 1,
                          &subResourceData);

    auto transition2 = CD3DX12_RESOURCE_BARRIER::Transition(
        defaultBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COMMON);
    m_commandList->ResourceBarrier(1, &transition2);

    m_uploadBuffers.push_back(uploadBuffer);

    return defaultBuffer;
}

void DefaultBufferUploader::Execute(ID3D12CommandQueue *pCommandQueue)
{
    m_commandList->Close();
    m_commandList.Execute(pCommandQueue);
}

} // namespace dxultra
