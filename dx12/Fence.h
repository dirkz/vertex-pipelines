#pragma once

#include "stdafx.h"

namespace zdx
{

struct Fence
{
    Fence(ID3D12Device *pDevice);
    ~Fence();

    void Signal(ID3D12CommandQueue *pCommandQueue);
    void Wait();

  private:
    ComPtr<ID3D12Fence> m_fence;
    UINT64 m_value;
    HANDLE m_event;
};

} // namespace zdx
