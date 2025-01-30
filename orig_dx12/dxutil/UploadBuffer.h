#pragma once

#include "stdafx.h"

namespace dxultra
{

template <class T, bool IsConstantBuffer> struct UploadBufferT
{
    UploadBufferT(ID3D12Device *pDevice, size_t numElements)
    {
        m_descriptorSize =
            pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        auto heapProperties = CD3DX12_HEAP_PROPERTIES{D3D12_HEAP_TYPE_UPLOAD};
        auto resourceDescription =
            CD3DX12_RESOURCE_DESC::Buffer(c_elementSizeAligned * numElements);
        ThrowIfFailed(pDevice->CreateCommittedResource(
            &heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDescription,
            D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(m_buffer.GetAddressOf())));

        ThrowIfFailed(m_buffer->Map(0, nullptr, reinterpret_cast<void **>(&m_mappedData)));
    }

    UploadBufferT(const UploadBufferT &other) = delete;
    UploadBufferT &operator=(const UploadBufferT &other) = delete;

    ~UploadBufferT()
    {
        if (m_buffer.Get())
        {
            m_buffer->Unmap(0, nullptr);
        }
    }

    UINT ElementSize()
    {
        return c_elementSizeAligned;
    }

    ID3D12Resource *Resource() const
    {
        return m_buffer.Get();
    }

    ID3D12Resource *operator->() const
    {
        return m_buffer.Get();
    }

    D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHandle(INT index)
    {
        return CD3DX12_CPU_DESCRIPTOR_HANDLE{m_buffer->GetCPUDescriptorHandleForHeapStart(), index,
                                             m_descriptorSize};
    }

    D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress(UINT index)
    {
        return m_buffer->GetGPUVirtualAddress() +
               static_cast<D3D12_GPU_VIRTUAL_ADDRESS>(index) * c_elementSizeAligned;
    }

    void CopyData(size_t elementIndex, const T &data)
    {
        memcpy(&m_mappedData[elementIndex * c_elementSizeAligned], &data, c_originalElementSize);
    }

  private:
    const UINT c_originalElementSize = sizeof(T);
    const UINT c_elementSizeAligned =
        IsConstantBuffer ? c_originalElementSize + 255 & ~255 : c_originalElementSize;

    ComPtr<ID3D12Resource> m_buffer;
    BYTE *m_mappedData;
    UINT m_descriptorSize;
};

template <class T> struct ConstantBuffer : public UploadBufferT<T, true>
{
    ConstantBuffer(ID3D12Device *pDevice, size_t numElements)
        : UploadBufferT<T, true>{pDevice, numElements}
    {
    }
};

template <class T> struct UploadBuffer : public UploadBufferT<T, false>
{
    UploadBuffer(ID3D12Device *pDevice, size_t numElements)
        : UploadBufferT<T, false>{pDevice, numElements}
    {
    }
};

} // namespace dxultra
