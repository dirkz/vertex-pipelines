#pragma once

#include "stdafx.h"

#include "Constants.h"
#include "DescriptorHeap.h"
#include "Frame.h"
#include "SwapChain.h"
#include "WindowCallback.h"
#include "Fence.h"

namespace zdx
{

struct DXWindow : WindowCallback
{
    static void GetHardwareAdapter(IDXGIFactory1 *pFactory, IDXGIAdapter1 **ppAdapter);

    void OnInit(HWND hwnd, UINT width, UINT height) override;
    void OnUpdate() override;
    void OnRender() override;

  private:
    void UploadBuffers();
    void RecordRenderCommands(CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle);

    ComPtr<ID3D12Device4> m_device;
    ComPtr<ID3D12CommandQueue> m_commandQueue;
    std::unique_ptr<SwapChain> m_swapChain;
    ComPtr<ID3D12GraphicsCommandList> m_commandList;
    std::array<std::unique_ptr<Frame>, NumFrames> m_frames;
    std::unique_ptr<Fence> m_mainFence;
    ComPtr<ID3D12Resource1> m_vertexBuffer;
    ComPtr<ID3D12Resource1> m_indexBuffer;
};

} // namespace zdx
