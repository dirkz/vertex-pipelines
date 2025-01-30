#pragma once

#include "stdafx.h"

namespace dxultra
{

/// <summary>
/// Bundles a <c>ID3D12Fence1</c> with the corresponding counter and <c>HANDLE</c>
/// for waiting for it.
/// </summary>
struct Fence
{
    Fence(ID3D12Device *pDevice);

    void Signal(ID3D12CommandQueue *pCommandQueue);
    void Wait();
    void SignalAndWait(ID3D12CommandQueue *pCommandQueue);

  private:
    UINT64 m_currentValue;
    ComPtr<ID3D12Fence1> m_fence;
    HANDLE m_event;
};

} // namespace dxultra