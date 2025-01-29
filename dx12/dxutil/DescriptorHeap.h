#pragma once

#include "stdafx.h"

namespace zdx
{

struct DescriptorHeap
{
    DescriptorHeap(ID3D12Device *pDevice, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors,
                   bool bShaderVisible);

    operator ID3D12DescriptorHeap *();

    D3D12_CPU_DESCRIPTOR_HANDLE HandleCPU(UINT index);
    D3D12_GPU_DESCRIPTOR_HANDLE HandleGPU(UINT index);

  private:
    ComPtr<ID3D12DescriptorHeap> m_descriptorHeap;
    D3D12_CPU_DESCRIPTOR_HANDLE m_hCPUStart;
    D3D12_GPU_DESCRIPTOR_HANDLE m_hGPUStart;
    UINT m_incrementSize;
};

} // namespace zdx
