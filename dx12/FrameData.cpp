#include "FrameData.h"

#include "Error.h"

namespace zdx
{

FrameData::FrameData(ID3D12Device *pDevice)
{
    ThrowIfFailed(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                  IID_PPV_ARGS(m_commandAllocator.GetAddressOf())));
}

} // namespace zdx