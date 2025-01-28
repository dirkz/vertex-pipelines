#pragma once

#include "stdafx.h"

#include "WindowCallback.h"

namespace zdx
{

struct DX12Window : WindowCallback
{
    static void GetHardwareAdapter(IDXGIFactory1 *pFactory, IDXGIAdapter1 **ppAdapter);

    void OnInit();

  private:
    ComPtr<ID3D12Device> m_device;
};

} // namespace zdx
