#include "Fence.h"

#include "Error.h"

namespace zdx
{

static constexpr UINT64 InitialFenceValue = 0;

Fence::Fence(ID3D12Device *pDevice)
{
    ThrowIfFailed(pDevice->CreateFence(InitialFenceValue, D3D12_FENCE_FLAG_NONE,
                                       IID_PPV_ARGS(m_fence.GetAddressOf())));

    m_value = InitialFenceValue;

    m_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (m_event == nullptr)
    {
        CheckLastError();
    }
}

Fence::~Fence()
{
    CloseHandle(m_event);
}

void Fence::Signal(ID3D12CommandQueue *pCommandQueue)
{
    m_value++;
    ThrowIfFailed(pCommandQueue->Signal(m_fence.Get(), m_value));
}

void Fence::Wait()
{
    UINT64 completedValue = m_fence->GetCompletedValue();

    if (completedValue == InitialFenceValue)
    {
        return;
    }

    if (completedValue < m_value)
    {
        ThrowIfFailed(m_fence->SetEventOnCompletion(m_value, m_event));
        WaitForSingleObject(m_event, INFINITE);
    }
}

void Fence::SignalAndWait(ID3D12CommandQueue *pCommandQueue)
{
    Signal(pCommandQueue);
    Wait();
}

} // namespace zdx