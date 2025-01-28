#pragma once

#include "stdafx.h"

#include "WindowCallback.h"
#include "DescriptorHeap.h"

namespace zdx
{

struct DX12Window : WindowCallback
{
    const UINT NumFrames = 2;

    static void GetHardwareAdapter(IDXGIFactory1 *pFactory, IDXGIAdapter1 **ppAdapter);

    void OnInit(HWND hwnd, UINT width, UINT height);

  private:
    ComPtr<ID3D12Device> m_device;
    ComPtr<ID3D12CommandQueue> m_commandQueue;
    ComPtr<IDXGISwapChain3> m_swapChain;
    std::unique_ptr<DescriptorHeap> m_renderTargetHeap;
};

} // namespace zdx
