#pragma once

#include "stdafx.h"

#include "CommandList.h"
#include "DrawData.h"
#include "Fence.h"
#include "MeshEntry.h"
#include "SwapChain.h"
#include "UploadBuffer.h"
#include "VertexPipeline.h"

namespace dxultra
{

struct Frame
{
    Frame(const DrawData *pDrawData, ID3D12Device *pDevice,
          const ComPtr<ID3D12RootSignature> rootSignature,
          const ComPtr<ID3D12PipelineState> pipelineState,
          const D3D12_VERTEX_BUFFER_VIEW &vertexBufferView,
          const D3D12_INDEX_BUFFER_VIEW &indexBufferView);

    void Draw(const DrawData *pDrawState, ID3D12CommandQueue *pCommandQueue,
              ID3D12GraphicsCommandList *pCommandList, SwapChain *pSwapChain,
              const D3D12_VIEWPORT &viewport, const D3D12_RECT &scissorRect);

  private:
    void CreateDescriptorHeaps(const DrawData *pDrawData, ID3D12Device *pDevice);

    Fence m_fence;
    ComPtr<ID3D12CommandAllocator> m_commandAllocator;

    ComPtr<ID3D12PipelineState> m_pipelineState;
    ComPtr<ID3D12RootSignature> m_rootSignature;

    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
    D3D12_INDEX_BUFFER_VIEW m_indexBufferView;

    UINT m_descriptorHeapIncrementSize;
    ComPtr<ID3D12DescriptorHeap> m_descriptorHeap;

    ConstantBuffer<DrawData::PerDrawPassDataObject> m_perDrawPassConstantBuffer;
    ConstantBuffer<XMMATRIX> m_perDrawCallConstantBuffer;
};

} // namespace dxultra