#include "SwapChain.h"
#include "Error.h"

namespace zdx
{

SwapChain::SwapChain(IDXGIFactory4 *pFactory, ID3D12Device *pDevice,
                     ID3D12CommandQueue *pCommandQueue, HWND hwnd, UINT width, UINT height)
    : m_renderTargetHeap{pDevice, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, NumFrames, false}
{
    // Describe and create the swap chain.
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = NumFrames;
    swapChainDesc.Width = width;
    swapChainDesc.Height = height;
    swapChainDesc.Format = RenderTargetFormat;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    ComPtr<IDXGISwapChain1> swapChain;
    ThrowIfFailed(pFactory->CreateSwapChainForHwnd(
        pCommandQueue, // Swap chain needs the queue so that it can force a flush on it.
        hwnd, &swapChainDesc, nullptr, nullptr, &swapChain));

    ThrowIfFailed(swapChain.As(&m_swapChain));

    for (UINT n = 0; n < NumFrames; n++)
    {
        ThrowIfFailed(m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n])));
        pDevice->CreateRenderTargetView(m_renderTargets[n].Get(), nullptr,
                                        m_renderTargetHeap.HandleCPU(n));
    }
}

UINT SwapChain::GetCurrentBackBufferIndex()
{
    return m_swapChain->GetCurrentBackBufferIndex();
}

ID3D12Resource *SwapChain::CurrentRenderTarget()
{
    return m_renderTargets[m_swapChain->GetCurrentBackBufferIndex()].Get();
}

CD3DX12_CPU_DESCRIPTOR_HANDLE SwapChain::CurrentRenderTargetDescriptorHandle()
{
    return m_renderTargetHeap.HandleCPU(m_swapChain->GetCurrentBackBufferIndex());
}

void SwapChain::Present(UINT syncInterval, UINT flags)
{
    ThrowIfFailed(m_swapChain->Present(syncInterval, flags));
}

} // namespace zdx