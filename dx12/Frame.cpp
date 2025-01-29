#include "Frame.h"

#include "Error.h"

namespace zdx
{

Frame::Frame(ID3D12Device *pDevice)
{
    ThrowIfFailed(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                  IID_PPV_ARGS(m_commandAllocator.GetAddressOf())));
}

ID3D12CommandAllocator *Frame::CommandAllocator()
{
    return m_commandAllocator.Get();
}

} // namespace zdx