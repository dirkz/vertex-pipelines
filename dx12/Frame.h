#pragma once

#include "stdafx.h"

namespace zdx
{

struct Frame
{
    /// <summary>
    /// Only for allowing arrays of empty <code>Frame</code>.
    /// </summary>
    Frame() {};

    Frame(ID3D12Device *pDevice);

    ID3D12CommandAllocator *CommandAllocator();

  private:
    ComPtr<ID3D12CommandAllocator> m_commandAllocator;
};

} // namespace zdx
