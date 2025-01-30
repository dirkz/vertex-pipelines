#pragma once

#include "stdafx.h"

namespace zdx
{

struct DefaultBuffer
{
    DefaultBuffer(ID3D12Device *pDevice, ID3D12GraphicsCommandList *pCommandList, UINT64 size,
                  const void *pData);

  private:
    ComPtr<ID3D12Resource1> m_defaultBuffer;
    ComPtr<ID3D12Resource1> m_uploadBuffer;
};

} // namespace zdx
