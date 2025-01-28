#pragma once

#include "stdafx.h"

namespace zdx
{

struct FrameData
{
    /// <summary>
    /// Only for allowing arrays of empty <code>FrameData</code>.
    /// </summary>
    FrameData() {};

    FrameData(ID3D12Device *pDevice);

  private:
    ComPtr<ID3D12CommandAllocator> m_commandAllocator;
};

} // namespace zdx
