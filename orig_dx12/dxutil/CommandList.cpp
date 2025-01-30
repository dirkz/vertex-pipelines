#include "CommandList.h"

namespace dxultra
{

CommandList::CommandList(ID3D12Device *pDevice)
{
    ThrowIfFailed(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                  IID_PPV_ARGS(m_commandAllocator.GetAddressOf())));
    ThrowIfFailed(pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
                                             m_commandAllocator.Get(), nullptr,
                                             IID_PPV_ARGS(m_commandList.GetAddressOf())));
}

void CommandList::Reset(ID3D12PipelineState *pInitialState)
{
    ThrowIfFailed(m_commandAllocator->Reset());
    ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), pInitialState));
}

void CommandList::Execute(ID3D12CommandQueue *pCommandQueue)
{
    ID3D12CommandList *commandLists[] = {Get()};
    pCommandQueue->ExecuteCommandLists(1, commandLists);
}

ID3D12GraphicsCommandList *CommandList::Get()
{
    return m_commandList.Get();
}

ID3D12GraphicsCommandList *CommandList::operator->()
{
    return m_commandList.Get();
}

} // namespace dxultra