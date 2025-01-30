#include "DXUltra.h"

#include "CommandList.h"
#include "DefaultBufferUploader.h"
#include "Geometry.h"
#include "Mesh.h"
#include "MeshEntry.h"
#include "Shader.h"
#include "VertexPipeline.h"

namespace dxultra
{

constexpr FLOAT ViewportInitialX = 0.0f;
constexpr FLOAT ViewportInitialY = 0.0f;
constexpr FLOAT ViewportInitialWidth = 0.0f;
constexpr FLOAT ViewportInitialHeight = 0.0f;
constexpr FLOAT ViewportMinDepth = 0.0f;
constexpr FLOAT ViewportMaxDepth = 1.0f;

DXUltra::DXUltra()
    : m_viewport{ViewportInitialX,      ViewportInitialY, ViewportInitialWidth,
                 ViewportInitialHeight, ViewportMinDepth, ViewportMaxDepth},
      m_projectionMatrix{}, m_scissorRect{}, m_vertexBufferView{}, m_indexBufferView{}
{
}

void DXUltra::CreatedWindow(HWND hwnd)
{
#ifdef _DEBUG
    ComPtr<ID3D12Debug> debugController;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
    {
        debugController->EnableDebugLayer();
    }
#endif

    UINT dxgiFactoryFlags = 0;
#ifdef _DEBUG
    dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif
    ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(m_factory.GetAddressOf())));

    ThrowIfFailed(m_factory->EnumAdapters1(0, m_adapter.GetAddressOf()));

    ThrowIfFailed(D3D12CreateDevice(m_adapter.Get(), D3D_FEATURE_LEVEL_12_0,
                                    IID_PPV_ARGS(m_device.GetAddressOf())));

    D3D12_COMMAND_QUEUE_DESC queueDesc{};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;

    ThrowIfFailed(
        m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_commandQueue.GetAddressOf())));

    ComPtr<ID3D12CommandAllocator> dummyCommandAllocator;
    ThrowIfFailed(m_device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(dummyCommandAllocator.GetAddressOf())));
    ThrowIfFailed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
                                              dummyCommandAllocator.Get(), nullptr,
                                              IID_PPV_ARGS(m_commandList.GetAddressOf())));
    m_commandList->Close();

    m_fence = std::make_unique<Fence>(m_device.Get());
    m_swapChain = std::make_unique<SwapChain>(FrameCount, hwnd, m_factory.Get(), m_device.Get(),
                                              m_commandQueue.Get());

    m_rootSignature = DrawData::CreateRootSignature(m_device.Get());

    CreatePipelineState();
    UploadTextureAndVertexData();

    for (UINT i = 0; i < FrameCount; ++i)
    {
        m_frames[i].reset(new Frame{m_drawData.get(), m_device.Get(), m_rootSignature,
                                    m_pipelineState, m_vertexBufferView, m_indexBufferView});
    }
}

void DXUltra::WillDestroyWindow(HWND hwnd)
{
    m_fence->SignalAndWait(m_commandQueue.Get());
}

void DXUltra::ResizeWindow(HWND hwnd, DWORD width, DWORD height)
{
    m_swapChain->Resize(hwnd, m_device.Get(), m_commandQueue.Get(), width, height);

    m_viewport.Height = height;
    m_viewport.Width = width;

    m_scissorRect.right = width;
    m_scissorRect.bottom = height;

    XMMATRIX p = XMMatrixPerspectiveFovLH(std::numbers::pi / 3, AspectRatio(), 1.0f, 1000.0f);
    XMStoreFloat4x4(&m_projectionMatrix, p);
}

inline static double AnimatedValue(long long time, long long period)
{
    double periodizedTime = static_cast<double>(time % period);
    return periodizedTime / static_cast<double>(period);
}

inline static double AnimatedValue(long long time, long long period, double minValue,
                                   double maxValue)
{
    double factor = AnimatedValue(time, period);
    return minValue + factor * (maxValue - minValue);
}

void DXUltra::Update(long long totalTime)
{
    XMVECTOR pos = XMVectorSet(0, 0, -5, 1);
    XMVECTOR target = XMVectorSet(0, 0, 0, 1);
    XMVECTOR up = XMVectorSet(0, 1, 0, 0);
    XMMATRIX view = XMMatrixLookAtLH(pos, target, up);

    double rotationAngle = AnimatedValue(totalTime, 15000, 0, 2 * std::numbers::pi);
    XMMATRIX rotationX = XMMatrixRotationX(rotationAngle);
    XMMATRIX rotationY = XMMatrixRotationY(rotationAngle);
    XMMATRIX rotationZ = XMMatrixRotationZ(rotationAngle);

    float apart = 2.5;
    XMMATRIX translation1 = XMMatrixTranslation(-apart, 0, 0);
    XMMATRIX translation2 = XMMatrixTranslation(apart, 0, 0);

    float scaleFactor = 2;
    XMMATRIX scale = XMMatrixScaling(scaleFactor, scaleFactor, scaleFactor);

    m_drawData->UpdateViewMatrix(XMMatrixTranspose(view));
    m_drawData->UpdateProjectionMatrix(XMMatrixTranspose(XMLoadFloat4x4(&m_projectionMatrix)));
    m_drawData->UpdatePerDrawCallData(
        0, XMMatrixTranspose(scale * rotationY * translation1));
    m_drawData->UpdatePerDrawCallData(
        1, XMMatrixTranspose(scale * rotationY * translation2));
}

void DXUltra::DrawWindow(HWND hwnd)
{
    UINT bufferIndex = m_swapChain->CurrentRenderTargetIndex();
    m_frames[bufferIndex]->Draw(m_drawData.get(), m_commandQueue.Get(), m_commandList.Get(),
                                m_swapChain.get(), m_viewport, m_scissorRect);
}

void DXUltra::CreatePipelineState()
{
    m_vertexPipeline.reset(new VertexPipeline{m_device.Get()});

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout = m_vertexPipeline->InputLayout();
    psoDesc.pRootSignature = m_rootSignature.Get();
    psoDesc.VS = CD3DX12_SHADER_BYTECODE{m_vertexPipeline->VertexShader()};
    psoDesc.PS = CD3DX12_SHADER_BYTECODE{m_vertexPipeline->PixelShader()};
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC{D3D12_DEFAULT};
    psoDesc.BlendState = CD3DX12_BLEND_DESC{D3D12_DEFAULT};
    psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC{D3D12_DEFAULT};
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = SwapChain::RenderTargetFormat;
    psoDesc.DSVFormat = SwapChain::DepthStencilBufferFormat;
    psoDesc.SampleDesc.Count = 1;

    ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)));
}

void DXUltra::UploadTextureAndVertexData()
{
    DefaultBufferUploader uploader{m_device};

    UploadTextureData(uploader);
    UploadVertexData(uploader);

    m_fence->SignalAndWait(m_commandQueue.Get());
}

void DXUltra::UploadTextureData(DefaultBufferUploader &uploader)
{
}

void DXUltra::UploadVertexData(DefaultBufferUploader &uploader)
{
    Geometry::MeshType mesh{};

    const double radius = 1;
    const UINT numElements = 10;
    MeshEntry entry1 = mesh.Add(Geometry::TriangleFan(1, 0.2, 20));
    MeshEntry entry2 = mesh.Add(Geometry::Sphere(1, 10, 10));

    m_vertexBuffer = uploader.CreateDefaultBuffer(mesh.Vertices());

    m_vertexBufferView = D3D12_VERTEX_BUFFER_VIEW{m_vertexBuffer->GetGPUVirtualAddress(),
                                                  mesh.VertexBufferSizeInBytes(), sizeof(Vertex)};

    m_indexBuffer = uploader.CreateDefaultBuffer(mesh.Indices());

    m_indexBufferView = D3D12_INDEX_BUFFER_VIEW{
        m_indexBuffer->GetGPUVirtualAddress(), mesh.IndexBufferSizeInBytes(), DXGI_FORMAT_R32_UINT};

    uploader.Execute(m_commandQueue.Get());

    m_drawData.reset(new DrawData{{entry1, entry2}});
}

} // namespace dxultra