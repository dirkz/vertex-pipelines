#pragma once

#include "stdafx.h"

namespace dxultra
{

struct CommandList
{
    CommandList(ID3D12Device *pDevice);
    CommandList(CommandList &other) = delete;

    void Reset(ID3D12PipelineState *pInitialState = nullptr);
    void Execute(ID3D12CommandQueue *pCommandQueue);

    ID3D12GraphicsCommandList *Get();
    ID3D12GraphicsCommandList *operator->();

  private:
    ComPtr<ID3D12CommandAllocator> m_commandAllocator;
    ComPtr<ID3D12GraphicsCommandList> m_commandList;
};

} // namespace dxultra
