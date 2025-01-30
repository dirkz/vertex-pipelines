#pragma once

#include "stdafx.h"

#include "CommandList.h"
#include "Fence.h"

namespace dxultra
{

struct SwapChain
{
    constexpr static DXGI_FORMAT RenderTargetFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    constexpr static DXGI_FORMAT DepthStencilBufferFormat = DXGI_FORMAT_D32_FLOAT;

    SwapChain(UINT frameCount, HWND hwnd, IDXGIFactory4 *pFactory, ID3D12Device *pDevice,
              ID3D12CommandQueue *pCommandQueue);

    UINT CurrentRenderTargetIndex() const;
    ID3D12Resource1 *CurrentRenderTargetBuffer();
    ID3D12Resource1 *DepthStencilBuffer();
    D3D12_CPU_DESCRIPTOR_HANDLE CurrentRenderTargetView();
    D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView();

    void Resize(HWND hwnd, ID3D12Device *pDevice, ID3D12CommandQueue *pCommandQueue, DWORD width,
                DWORD height);

    HRESULT Present(UINT syncInterval, UINT flags);

  private:
    void CreateDescriptorHeaps(ID3D12Device *pDevice);
    void CreateRenderTargetViews(ID3D12Device *pDevice);

    /// <summary>
    /// Creates a depth (no stencil so far) buffer and puts a transition into the
    /// command list.
    /// </summary>
    /// <param name="pDevice"></param>
    /// <param name="width"></param>
    /// <param name="height"></param>
    void CreateDepthStencilBufferAndView(ID3D12Device *pDevice, DWORD width, DWORD height);

    UINT m_frameCount;
    Fence m_fence;

    CommandList m_commandList;

    ComPtr<IDXGISwapChain3> m_swapChain;
    ComPtr<ID3D12Resource1> m_dsvBuffer;

    UINT m_rtvDescriptorSize = 0;
    ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
    ComPtr<ID3D12DescriptorHeap> m_dsvHeap;

    std::vector<ComPtr<ID3D12Resource1>> m_rtvBuffers;
};

} // namespace dxultra