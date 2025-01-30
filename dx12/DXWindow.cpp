#include "DXWindow.h"

#include "DefaultBuffer.h"
#include "Error.h"
#include "Shader.h"
#include "Vertex.h"

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

    m_mainFence.reset(new Fence{m_device.Get()});

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

    CreateRootSignature();
    UploadBuffers();
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

    ID3D12Resource *pRenderTarget = m_swapChain->CurrentRenderTarget();
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_swapChain->CurrentRenderTargetDescriptorHandle();

    auto transitionPresentToRenderTarget = CD3DX12_RESOURCE_BARRIER::Transition(
        pRenderTarget, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

    // Indicate that the back buffer will be used as a render target.
    m_commandList->ResourceBarrier(1, &transitionPresentToRenderTarget);

    RecordRenderCommands(rtvHandle);

    auto transitionRenderTargetToPresent = CD3DX12_RESOURCE_BARRIER::Transition(
        pRenderTarget, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

    // Indicate that the back buffer will now be used to present.
    m_commandList->ResourceBarrier(1, &transitionRenderTargetToPresent);

    ThrowIfFailed(m_commandList->Close());

    // Execute the command list.
    ID3D12CommandList *ppCommandLists[] = {m_commandList.Get()};
    m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    // Present the frame.
    m_swapChain->Present(1, 0);

    pFrame->Signal(m_commandQueue.Get());
}

void DXWindow::CreateRootSignature()
{
    D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

    // This is the highest version supported. If CheckFeatureSupport succeeds, the
    // HighestVersion returned will not be greater than this.
    featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

    if (FAILED(m_device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData,
                                             sizeof(featureData))))
    {
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
    }

    CD3DX12_DESCRIPTOR_RANGE1 range1{};
    CD3DX12_DESCRIPTOR_RANGE1 range2{};
    CD3DX12_ROOT_PARAMETER1 rootParameters[2]{};

    range1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
    range2.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
    rootParameters[0].InitAsDescriptorTable(1, &range1, D3D12_SHADER_VISIBILITY_VERTEX);
    rootParameters[1].InitAsDescriptorTable(1, &range2, D3D12_SHADER_VISIBILITY_VERTEX);

    // Allow input layout and deny uneccessary access to certain pipeline stages.
    D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc{};
    rootSignatureDesc.Init_1_1(2, rootParameters, 0, nullptr, rootSignatureFlags);

    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;
    HRESULT hResult = D3DX12SerializeVersionedRootSignature(
        &rootSignatureDesc, featureData.HighestVersion, &signature, &error);

    if (!SUCCEEDED(hResult) && error.Get() && error->GetBufferSize() && error->GetBufferPointer())
    {
        OutputDebugStringA(reinterpret_cast<const char *>(error->GetBufferPointer()));
    }
    ThrowIfFailed(hResult);

    ThrowIfFailed(m_device->CreateRootSignature(0, signature->GetBufferPointer(),
                                                signature->GetBufferSize(),
                                                IID_PPV_ARGS(m_rootSignature.GetAddressOf())));
}

void DXWindow::UploadBuffers()
{
    Shader vertexShader{L"shaders\\basic_triangle.hlsl_VS.cso", "VS", "vs_5_1"};
    Shader pixelShader{L"shaders\\basic_triangle.hlsl_PS.cso", "PS", "vs_5_1"};

    std::array<Vertex, 3> vertices{Vertex{-0.5, -0.5, 0, Colors::Red},
                                   Vertex{0.0, 0.5, 0, Colors::Green},
                                   Vertex{0.5, -0.5, 0, Colors::Blue}};

    std::array<UINT16, 3> indices{0, 1, 2};

    ComPtr<ID3D12CommandAllocator> commandAllocator;
    ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                   IID_PPV_ARGS(commandAllocator.GetAddressOf())));

    ThrowIfFailed(commandAllocator->Reset());
    ThrowIfFailed(m_commandList->Reset(commandAllocator.Get(), nullptr));

    DefaultBuffer vertexBuffer{m_device.Get(), m_commandList.Get(), vertices.size(),
                               vertices.data()};
    DefaultBuffer indexBuffer{m_device.Get(), m_commandList.Get(), indices.size(), indices.data()};

    ThrowIfFailed(m_commandList->Close());

    // Execute the command list.
    ID3D12CommandList *ppCommandLists[] = {m_commandList.Get()};
    m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    m_mainFence->SignalAndWait(m_commandQueue.Get());

    m_vertexBuffer = vertexBuffer.Buffer();
    m_indexBuffer = indexBuffer.Buffer();
}

void DXWindow::RecordRenderCommands(CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle)
{
    const float clearColor[] = {0.0f, 0.2f, 0.4f, 1.0f};
    m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
}

} // namespace zdx
