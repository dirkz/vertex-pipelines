#pragma once

#include "stdafx.h"

#include "WindowCallback.h"

namespace zdx
{

struct DX12Window : WindowCallback
{
    static void GetHardwareAdapter(IDXGIFactory1 *pFactory, IDXGIAdapter1 **ppAdapter);

    void OnInit(HWND hwnd, UINT width, UINT height);

  private:
    ComPtr<ID3D12Device> m_device;
    ComPtr<ID3D12CommandQueue> m_commandQueue;
};

} // namespace zdx
