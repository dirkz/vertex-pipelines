#pragma once

#include "stdafx.h"

#include "Constants.h"
#include "DescriptorHeap.h"

namespace zdx
{

struct SwapChain
{
    constexpr static DXGI_FORMAT RenderTargetFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    constexpr static DXGI_FORMAT DepthStencilBufferFormat = DXGI_FORMAT_D32_FLOAT;
    
    SwapChain(IDXGIFactory4 *pFactory, ID3D12Device *pDevice, ID3D12CommandQueue *pCommandQueue,
              HWND hwnd, UINT width, UINT height);

    UINT GetCurrentBackBufferIndex();
    ID3D12Resource *CurrentRenderTarget();
    CD3DX12_CPU_DESCRIPTOR_HANDLE CurrentRenderTargetDescriptorHandle();

    void Present(UINT syncInterval, UINT flags);

  private:
    ComPtr<IDXGISwapChain3> m_swapChain;
    DescriptorHeap m_renderTargetHeap;
    std::array<ComPtr<ID3D12Resource>, NumFrames> m_renderTargets;
};

} // namespace zdx
