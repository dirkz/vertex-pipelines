#pragma once

#include "stdafx.h"

#include "DescriptorHeap.h"

namespace zdx
{

struct SwapChain
{
    SwapChain(UINT numFrames, IDXGIFactory4 *pFactory, ID3D12Device *pDevice,
              ID3D12CommandQueue *pCommandQueue, HWND hwnd, UINT width, UINT height);

  private:
    ComPtr<IDXGISwapChain3> m_swapChain;
    std::unique_ptr<DescriptorHeap> m_renderTargetHeap;
    std::vector<ComPtr<ID3D12Resource>> m_renderTargets;
};

}; // namespace zdx
