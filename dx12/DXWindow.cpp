#include "DXWindow.h"

#include "Error.h"

namespace zdx
{

void DXWindow::GetHardwareAdapter(IDXGIFactory1 *pFactory, IDXGIAdapter1 **ppAdapter)
{
    *ppAdapter = nullptr;

    ComPtr<IDXGIAdapter1> adapter;

    ComPtr<IDXGIFactory6> factory6;
    if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(factory6.GetAddressOf()))))
    {
        for (UINT adapterIndex = 0; SUCCEEDED(
                 factory6->EnumAdapterByGpuPreference(adapterIndex, DXGI_GPU_PREFERENCE_UNSPECIFIED,
                                                      IID_PPV_ARGS(adapter.GetAddressOf())));
             ++adapterIndex)
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                // No software rendering.
                continue;
            }

            // Check to see whether the adapter supports Direct3D 12, but don't create the
            // actual device yet.
            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0,
                                            _uuidof(ID3D12Device), nullptr)))
            {
                break;
            }
        }
    }

    if (adapter.Get() == nullptr)
    {
        for (UINT adapterIndex = 0;
             SUCCEEDED(pFactory->EnumAdapters1(adapterIndex, adapter.GetAddressOf()));
             ++adapterIndex)
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                // No software rendering.
                continue;
            }

            // Check to see whether the adapter supports Direct3D 12, but don't create the
            // actual device yet.
            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0,
                                            _uuidof(ID3D12Device), nullptr)))
            {
                break;
            }
        }
    }

    *ppAdapter = adapter.Detach();
}

void DXWindow::OnInit(HWND hwnd, UINT width, UINT height)
{
    OutputDebugString(L"DX12Window::OnInit()\n");

    UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
    // Enable the debug layer (requires the Graphics Tools "optional feature").
    // NOTE: Enabling the debug layer after device creation will invalidate the active device.
    {
        ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debugController.GetAddressOf()))))
        {
            debugController->EnableDebugLayer();

            // Enable additional debug layers.
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }
    }
#endif

    ComPtr<IDXGIFactory4> factory;
    ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(factory.GetAddressOf())));

    ComPtr<IDXGIAdapter1> hardwareAdapter;
    GetHardwareAdapter(factory.Get(), hardwareAdapter.GetAddressOf());

    ComPtr<ID3D12Device> device;
    ThrowIfFailed(D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0,
                                    IID_PPV_ARGS(device.GetAddressOf())));

    ThrowIfFailed(device.As(&m_device));

    // Describe and create the command queue.
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    ThrowIfFailed(
        m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_commandQueue.GetAddressOf())));

    m_swapChain = std::make_unique<SwapChain>(factory.Get(), m_device.Get(), m_commandQueue.Get(),
                                              hwnd, width, height);

    // Don't allow fullscreen transitions.
    ThrowIfFailed(factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER));

    ThrowIfFailed(m_device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
                                               D3D12_COMMAND_LIST_FLAG_NONE,
                                               IID_PPV_ARGS(m_commandList.GetAddressOf())));

    for (UINT n = 0; n < NumFrames; ++n)
    {
        m_frames[n].reset(new Frame{m_device.Get()});
    }
}

void DXWindow::OnUpdate()
{
}

void DXWindow::OnRender()
{
    UINT frameIndex = m_swapChain->GetCurrentBackBufferIndex();
    Frame *pFrame = m_frames[frameIndex].get();

    // Wait for any previous work for this frame
    // (that has been scheduled on the command queue).
    pFrame->Wait();

    ThrowIfFailed(pFrame->CommandAllocator()->Reset());
    ThrowIfFailed(m_commandList->Reset(pFrame->CommandAllocator(), nullptr));
    ThrowIfFailed(m_commandList->Close());

    pFrame->Signal(m_commandQueue.Get());
}

} // namespace zdx
