#include "SwapChain.h"
#include "Error.h"

namespace zdx
{

SwapChain::SwapChain(UINT numFrames, IDXGIFactory4 *pFactory, ID3D12Device *pDevice,
                     ID3D12CommandQueue *pCommandQueue, HWND hwnd, UINT width, UINT height)
    : m_renderTargetHeap{pDevice, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, numFrames, false},
      m_renderTargets(numFrames)
{
    // Describe and create the swap chain.
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = numFrames;
    swapChainDesc.Width = width;
    swapChainDesc.Height = height;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    ComPtr<IDXGISwapChain1> swapChain;
    ThrowIfFailed(pFactory->CreateSwapChainForHwnd(
        pCommandQueue, // Swap chain needs the queue so that it can force a flush on it.
        hwnd, &swapChainDesc, nullptr, nullptr, &swapChain));

    ThrowIfFailed(swapChain.As(&m_swapChain));

    for (UINT n = 0; n < numFrames; n++)
    {
        ThrowIfFailed(m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n])));
        pDevice->CreateRenderTargetView(m_renderTargets[n].Get(), nullptr,
                                        m_renderTargetHeap.HandleCPU(n));
    }
}

} // namespace zdx