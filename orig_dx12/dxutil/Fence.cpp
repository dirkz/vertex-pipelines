#include "Fence.h"

namespace dxultra
{

Fence::Fence(ID3D12Device *pDevice) : m_currentValue{0}
{
    ThrowIfFailed(pDevice->CreateFence(m_currentValue, D3D12_FENCE_FLAG_NONE,
                                       IID_PPV_ARGS(m_fence.GetAddressOf())));
    m_event = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
    if (!m_event)
    {
        throw std::runtime_error{"Could not create HANDLE for waiting on a fence"};
    }
}

void Fence::Signal(ID3D12CommandQueue *pCommandQueue)
{
    ++m_currentValue;
    pCommandQueue->Signal(m_fence.Get(), m_currentValue);
}

void Fence::Wait()
{
    if (m_currentValue != 0 && m_fence->GetCompletedValue() < m_currentValue)
    {
        m_fence->SetEventOnCompletion(m_currentValue, m_event);
        WaitForSingleObject(m_event, INFINITE);
    }
}

void Fence::SignalAndWait(ID3D12CommandQueue *pCommandQueue)
{
    Signal(pCommandQueue);
    Wait();
}

} // namespace dxultra