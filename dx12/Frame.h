#pragma once

#include "stdafx.h"

#include "Fence.h"

namespace zdx
{

struct Frame
{
    Frame(ID3D12Device *pDevice);

    ID3D12CommandAllocator *CommandAllocator();

    void Signal(ID3D12CommandQueue *pCommandQueue);
    void Wait();

  private:
    ComPtr<ID3D12CommandAllocator> m_commandAllocator;
    Fence m_fence;
};

} // namespace zdx
