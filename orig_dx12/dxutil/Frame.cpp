#include "Frame.h"

#include "SwapChain.h"

namespace dxultra
{

using namespace DirectX;

Frame::Frame(const DrawData *pDrawData, ID3D12Device *pDevice,
             const ComPtr<ID3D12RootSignature> rootSignature,
             const ComPtr<ID3D12PipelineState> pipelineState,
             const D3D12_VERTEX_BUFFER_VIEW &vertexBufferView,
             const D3D12_INDEX_BUFFER_VIEW &indexBufferView)
    : m_fence{pDevice}, m_pipelineState{pipelineState}, m_rootSignature{rootSignature},
      m_vertexBufferView{vertexBufferView}, m_indexBufferView{indexBufferView},
      m_perDrawPassConstantBuffer{pDevice, 1},
      m_perDrawCallConstantBuffer{pDevice, pDrawData->NumObjects()}
{
    ThrowIfFailed(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                  IID_PPV_ARGS(m_commandAllocator.GetAddressOf())));
    CreateDescriptorHeaps(pDrawData, pDevice);
}

void Frame::Draw(const DrawData *pDrawData, ID3D12CommandQueue *pCommandQueue,
                 ID3D12GraphicsCommandList *pCommandList, SwapChain *pSwapChain,
                 const D3D12_VIEWPORT &viewport, const D3D12_RECT &scissorRect)
{
    // Wait for the previous round (of this frame) to be finished.
    m_fence.Wait();

    m_commandAllocator->Reset();
    pCommandList->Reset(m_commandAllocator.Get(), m_pipelineState.Get());

    m_perDrawPassConstantBuffer.CopyData(0, pDrawData->PerDrawPassData());

    for (UINT i = 0; i < pDrawData->NumObjects(); ++i)
    {
        m_perDrawCallConstantBuffer.CopyData(i, pDrawData->PerDrawCallData(i));
    }

    auto transition1 = CD3DX12_RESOURCE_BARRIER::Transition(pSwapChain->CurrentRenderTargetBuffer(),
                                                            D3D12_RESOURCE_STATE_PRESENT,
                                                            D3D12_RESOURCE_STATE_RENDER_TARGET);
    pCommandList->ResourceBarrier(1, &transition1);

    pCommandList->RSSetViewports(1, &viewport);
    pCommandList->RSSetScissorRects(1, &scissorRect);

    pCommandList->ClearRenderTargetView(pSwapChain->CurrentRenderTargetView(), Colors::Black, 0,
                                        nullptr);
    pCommandList->ClearDepthStencilView(pSwapChain->DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH,
                                        1.0f, 0, 0, nullptr);

    pCommandList->SetGraphicsRootSignature(m_rootSignature.Get());

    ID3D12DescriptorHeap *ppHeaps[] = {m_descriptorHeap.Get()};
    pCommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

    pCommandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
    pCommandList->IASetIndexBuffer(&m_indexBufferView);

    pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    D3D12_CPU_DESCRIPTOR_HANDLE renderTargetView = pSwapChain->CurrentRenderTargetView();
    D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView = pSwapChain->DepthStencilView();
    pCommandList->OMSetRenderTargets(1, &renderTargetView, TRUE, &depthStencilView);

    pCommandList->SetGraphicsRootDescriptorTable(
        0, m_descriptorHeap->GetGPUDescriptorHandleForHeapStart());

    // Point to the first per-draw-call descriptor.
    CD3DX12_GPU_DESCRIPTOR_HANDLE handle{m_descriptorHeap->GetGPUDescriptorHandleForHeapStart(), 1,
                                         m_descriptorHeapIncrementSize};

    for (UINT i = 0; i < pDrawData->NumObjects(); ++i)
    {
        const MeshEntry &meshEntry = pDrawData->MeshEntries()[i];

        pCommandList->SetGraphicsRootDescriptorTable(1, handle);
        pCommandList->DrawIndexedInstanced(meshEntry.NumIndices(), 1, meshEntry.IndexOffset(),
                                           meshEntry.VertexOffset(), 0);

        // Move to the next per-draw-call descriptor.
        handle.Offset(1, m_descriptorHeapIncrementSize);
    }

    auto transition2 = CD3DX12_RESOURCE_BARRIER::Transition(pSwapChain->CurrentRenderTargetBuffer(),
                                                            D3D12_RESOURCE_STATE_RENDER_TARGET,
                                                            D3D12_RESOURCE_STATE_PRESENT);
    pCommandList->ResourceBarrier(1, &transition2);

    ThrowIfFailed(pCommandList->Close());

    ID3D12CommandList *commandLists[] = {pCommandList};
    pCommandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

    ThrowIfFailed(pSwapChain->Present(0, 0));

    m_fence.Signal(pCommandQueue);
}

void Frame::CreateDescriptorHeaps(const DrawData *pDrawData, ID3D12Device *pDevice)
{
    m_descriptorHeapIncrementSize =
        pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    // Create the descriptor heap.
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
    heapDesc.NumDescriptors = 1 + pDrawData->NumObjects();
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    ThrowIfFailed(
        pDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(m_descriptorHeap.GetAddressOf())));

    // Let the first descriptor point to the per-draw-pass constant buffer.
    {
        D3D12_CONSTANT_BUFFER_VIEW_DESC cbDesc{};
        cbDesc.BufferLocation = m_perDrawPassConstantBuffer.GetGPUVirtualAddress(0);
        cbDesc.SizeInBytes = m_perDrawPassConstantBuffer.ElementSize();

        pDevice->CreateConstantBufferView(&cbDesc,
                                          m_descriptorHeap->GetCPUDescriptorHandleForHeapStart());
    }

    // Let the subsequent (second etc.) descriptors point to subsequent elements of
    // the per-draw-call constant buffer.
    for (INT i = 0; i < pDrawData->NumObjects(); ++i)
    {
        // The GPU address of the per-object buffer.
        D3D12_CONSTANT_BUFFER_VIEW_DESC cbDesc{};
        cbDesc.BufferLocation = m_perDrawCallConstantBuffer.GetGPUVirtualAddress(i);
        cbDesc.SizeInBytes = m_perDrawCallConstantBuffer.ElementSize();

        CD3DX12_CPU_DESCRIPTOR_HANDLE handle{m_descriptorHeap->GetCPUDescriptorHandleForHeapStart(),
                                             i + 1, m_descriptorHeapIncrementSize};

        pDevice->CreateConstantBufferView(&cbDesc, handle);
    }
}

} // namespace dxultra