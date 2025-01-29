#include "Fence.h"

#include "Error.h"

namespace zdx
{

Fence::Fence(ID3D12Device *pDevice)
{
    UINT64 value = 0;

    ThrowIfFailed(
        pDevice->CreateFence(value, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_fence.GetAddressOf())));

    m_value = value + 1;

    m_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (m_event == nullptr)
    {
        ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
    }
}

Fence::~Fence()
{
    CloseHandle(m_event);
}

void Fence::Signal(ID3D12CommandQueue *pCommandQueue)
{
    ThrowIfFailed(pCommandQueue->Signal(m_fence.Get(), m_value));
    m_value++;
}

void Fence::Wait()
{

    if (m_fence->GetCompletedValue() < m_value)
    {
        ThrowIfFailed(m_fence->SetEventOnCompletion(m_value, m_event));
        WaitForSingleObject(m_event, INFINITE);
    }
}

} // namespace zdx