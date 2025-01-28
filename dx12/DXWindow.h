#pragma once

#include "stdafx.h"

#include "WindowCallback.h"
#include "DescriptorHeap.h"
#include "SwapChain.h"

namespace zdx
{

struct DXWindow : WindowCallback
{
    const UINT NumFrames = 2;

    static void GetHardwareAdapter(IDXGIFactory1 *pFactory, IDXGIAdapter1 **ppAdapter);

    void OnInit(HWND hwnd, UINT width, UINT height);

  private:
    ComPtr<ID3D12Device4> m_device;
    ComPtr<ID3D12CommandQueue> m_commandQueue;
    std::unique_ptr<SwapChain> m_swapChain;
};

} // namespace zdx
