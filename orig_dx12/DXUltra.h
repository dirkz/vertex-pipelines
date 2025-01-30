#pragma once

#include "stdafx.h"

#include "DefaultBufferUploader.h"
#include "DrawData.h"
#include "Fence.h"
#include "Frame.h"
#include "SwapChain.h"
#include "UploadBuffer.h"
#include "VertexPipeline.h"
#include "Window.h"

namespace dxultra
{

struct DXUltra : public Window
{
    DXUltra();

  protected:
    void CreatedWindow(HWND hwnd) override;
    void WillDestroyWindow(HWND hwnd) override;
    void ResizeWindow(HWND hwnd, DWORD width, DWORD height) override;
    void Update(long long totalTime) override;
    void DrawWindow(HWND hwnd) override;

  private:
    static constexpr UINT FrameCount = 2;

    void CreatePipelineState();
    void UploadTextureAndVertexData();
    void UploadTextureData(DefaultBufferUploader &uploader);
    void UploadVertexData(DefaultBufferUploader &uploader);

    D3D12_VIEWPORT m_viewport;
    D3D12_RECT m_scissorRect;
    XMFLOAT4X4 m_projectionMatrix;

    ComPtr<IDXGIFactory4> m_factory;
    ComPtr<IDXGIAdapter1> m_adapter;
    ComPtr<ID3D12Device> m_device;
    ComPtr<ID3D12CommandQueue> m_commandQueue;
    ComPtr<ID3D12GraphicsCommandList> m_commandList;
    ComPtr<ID3D12RootSignature> m_rootSignature;

    std::unique_ptr<VertexPipeline> m_vertexPipeline;
    ComPtr<ID3D12PipelineState> m_pipelineState;

    ComPtr<ID3D12Resource1> m_vertexBuffer;
    ComPtr<ID3D12Resource1> m_indexBuffer;
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
    D3D12_INDEX_BUFFER_VIEW m_indexBufferView;

    std::unique_ptr<Fence> m_fence;
    std::unique_ptr<SwapChain> m_swapChain;

    std::array<std::unique_ptr<Frame>, FrameCount> m_frames;

    std::unique_ptr<DrawData> m_drawData;
};

} // namespace dxultra