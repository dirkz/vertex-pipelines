#include "DescriptorHeap.h"

#include "Error.h"

namespace zdx
{

DescriptorHeap::DescriptorHeap(ID3D12Device *pDevice, D3D12_DESCRIPTOR_HEAP_TYPE heapType,
                               UINT numDescriptors, bool bShaderVisible)
{
    D3D12_DESCRIPTOR_HEAP_DESC desc{};
    desc.Type = heapType;
    desc.NumDescriptors = numDescriptors;
    desc.Flags = (bShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
                                 : D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

    ThrowIfFailed(
        pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_descriptorHeap.GetAddressOf())));

    m_hCPUStart = m_descriptorHeap->GetCPUDescriptorHandleForHeapStart();

    if (bShaderVisible)
    {
        m_hGPUStart = m_descriptorHeap->GetGPUDescriptorHandleForHeapStart();
    }
    else
    {
        m_hGPUStart = CD3DX12_GPU_DESCRIPTOR_HANDLE{};
    }

    m_incrementSize = pDevice->GetDescriptorHandleIncrementSize(heapType);
}

DescriptorHeap::operator ID3D12DescriptorHeap *()
{
    return m_descriptorHeap.Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::HandleCPU(UINT index)
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_hCPUStart, index, m_incrementSize);
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::HandleGPU(UINT index)
{
    return CD3DX12_GPU_DESCRIPTOR_HANDLE(m_hGPUStart, index, m_incrementSize);
}

} // namespace zdx