#pragma once

#include "stdafx.h"

#include "CommandList.h"

namespace dxultra
{

/// <summary>
/// Methods for creating default buffers from given data.
/// The temporary upload buffers will be destroyed in the destructor,
/// so make sure you wait for the GPU to finish the copying.
/// </summary>
struct DefaultBufferUploader
{
    DefaultBufferUploader(ComPtr<ID3D12Device> device);

    ComPtr<ID3D12Resource1> CreateDefaultBuffer(
        const void *pData, UINT64 size,
        D3D12_RESOURCE_STATES targetResourceState = D3D12_RESOURCE_STATE_GENERIC_READ);

    template <class T> ComPtr<ID3D12Resource1> CreateDefaultBuffer(const std::vector<T> &v)
    {
        return CreateDefaultBuffer(v.data(), v.size() * sizeof(T));
    }

    template <class T> ComPtr<ID3D12Resource1> CreateDefaultBuffer(const std::span<T> &span)
    {
        return CreateDefaultBuffer(span.data(), span.size() * sizeof(T));
    }

    template <class T> ComPtr<ID3D12Resource1> CreateDefaultBuffer(const T &data)
    {
        return CreateDefaultBuffer(&data, sizeof(data));
    }

    void Execute(ID3D12CommandQueue *pCommandQueue);

  private:
    CommandList m_commandList;
    ComPtr<ID3D12Device> m_device;

    std::vector<ComPtr<ID3D12Resource1>> m_uploadBuffers;
};

} // namespace dxultra
