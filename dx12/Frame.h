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

  private:
    ComPtr<ID3D12CommandAllocator> m_commandAllocator;
};

} // namespace zdx
