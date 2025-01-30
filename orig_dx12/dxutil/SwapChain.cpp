#include "SwapChain.h"

namespace dxultra
{

SwapChain::SwapChain(UINT frameCount, HWND hwnd, IDXGIFactory4 *pFactory, ID3D12Device *pDevice,
                     ID3D12CommandQueue *pCommandQueue)
    : m_frameCount{frameCount}, m_fence{pDevice}, m_rtvBuffers(frameCount), m_commandList{pDevice}
{
    // no recording, close the command list so it can be reset
    m_commandList->Close();

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
    swapChainDesc.BufferCount = frameCount;
    swapChainDesc.Format = RenderTargetFormat;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc = {1, 0};

    ComPtr<IDXGISwapChain1> swapChain;
    ThrowIfFailed(pFactory->CreateSwapChainForHwnd(pCommandQueue, hwnd, &swapChainDesc, nullptr,
                                                   nullptr, swapChain.GetAddressOf()));

    ThrowIfFailed(swapChain.As(&m_swapChain));

    m_rtvDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    CreateDescriptorHeaps(pDevice);
}

UINT SwapChain::CurrentRenderTargetIndex() const
{
    return m_swapChain->GetCurrentBackBufferIndex();
}

ID3D12Resource1 *SwapChain::CurrentRenderTargetBuffer()
{
    return m_rtvBuffers[CurrentRenderTargetIndex()].Get();
}

ID3D12Resource1 *SwapChain::DepthStencilBuffer()
{
    return m_dsvBuffer.Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE SwapChain::CurrentRenderTargetView()
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE{m_rtvHeap->GetCPUDescriptorHandleForHeapStart(),
                                         static_cast<INT>(CurrentRenderTargetIndex()),
                                         m_rtvDescriptorSize};
}

D3D12_CPU_DESCRIPTOR_HANDLE SwapChain::DepthStencilView()
{
    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle{m_dsvHeap->GetCPUDescriptorHandleForHeapStart()};
    return dsvHandle;
}

void SwapChain::Resize(HWND hwnd, ID3D12Device *pDevice, ID3D12CommandQueue *pCommandQueue,
                       DWORD width, DWORD height)
{
    // Wait for the end of ALL commands currently in the queue.
    m_fence.SignalAndWait(pCommandQueue);

    // Bring the command list (and its allocator) into a state
    // where new commands can be recorded.
    m_commandList.Reset();

    // Destroy the referenced buffers.
    for (UINT i = 0; i < m_frameCount; ++i)
    {
        m_rtvBuffers[i].Reset();
    }

    ThrowIfFailed(m_swapChain->ResizeBuffers(m_frameCount, width, height, RenderTargetFormat, 0));

    CreateRenderTargetViews(pDevice);

    // This will write a transition into the command list.
    CreateDepthStencilBufferAndView(pDevice, width, height);

    m_commandList->Close();
    m_commandList.Execute(pCommandQueue);
}

HRESULT SwapChain::Present(UINT syncInterval, UINT flags)
{
    return m_swapChain->Present(syncInterval, flags);
}

void SwapChain::CreateDescriptorHeaps(ID3D12Device *pDevice)
{
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
    heapDesc.NumDescriptors = m_frameCount;
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    heapDesc.NodeMask = 0;

    ThrowIfFailed(pDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_rtvHeap)));

    heapDesc.NumDescriptors = 1;
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

    ThrowIfFailed(pDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_dsvHeap)));
}

void SwapChain::CreateRenderTargetViews(ID3D12Device *pDevice)
{
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle{m_rtvHeap->GetCPUDescriptorHandleForHeapStart()};
    for (UINT i = 0; i < m_frameCount; ++i)
    {
        // Get the buffer/resource.
        ThrowIfFailed(m_swapChain->GetBuffer(i, IID_PPV_ARGS(m_rtvBuffers[i].GetAddressOf())));

        // Create the view/handle/descriptor.
        pDevice->CreateRenderTargetView(m_rtvBuffers[i].Get(), nullptr, rtvHandle);

        // Increase the descriptor.
        rtvHandle.Offset(1, m_rtvDescriptorSize);
    }
}

void SwapChain::CreateDepthStencilBufferAndView(ID3D12Device *pDevice, DWORD width, DWORD height)
{
    D3D12_RESOURCE_DESC dsvDesc{};
    dsvDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    dsvDesc.Alignment = 0;
    dsvDesc.Width = width;
    dsvDesc.Height = height;
    dsvDesc.DepthOrArraySize = 1;
    dsvDesc.MipLevels = 1;
    dsvDesc.Format = DepthStencilBufferFormat;
    dsvDesc.SampleDesc = {1, 0};
    dsvDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    dsvDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE clearValue{};
    clearValue.Format = DepthStencilBufferFormat;
    clearValue.DepthStencil.Depth = 1.0f;
    clearValue.DepthStencil.Stencil = 0;

    CD3DX12_HEAP_PROPERTIES heapProperties{D3D12_HEAP_TYPE_DEFAULT};

    ThrowIfFailed(pDevice->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &dsvDesc,
                                                   D3D12_RESOURCE_STATE_COMMON, &clearValue,
                                                   IID_PPV_ARGS(&m_dsvBuffer)));

    pDevice->CreateDepthStencilView(m_dsvBuffer.Get(), nullptr,
                                    m_dsvHeap->GetCPUDescriptorHandleForHeapStart());

    auto transition = CD3DX12_RESOURCE_BARRIER::Transition(
        m_dsvBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    m_commandList.Get()->ResourceBarrier(1, &transition);
}

} // namespace dxultra
