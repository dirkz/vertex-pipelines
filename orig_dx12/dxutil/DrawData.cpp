#include "DrawData.h"

namespace dxultra
{

DrawData::PerDrawPassDataObject::PerDrawPassDataObject(CXMMATRIX viewMatrix,
                                                       CXMMATRIX projectionMatrix)
{
    XMStoreFloat4x4(&ViewMatrix, viewMatrix);
    XMStoreFloat4x4(&ProjectionMatrix, projectionMatrix);
}

ComPtr<ID3D12RootSignature> DrawData::CreateRootSignature(ID3D12Device *pDevice)
{
    D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

    // This is the highest version supported. If CheckFeatureSupport succeeds, the
    // HighestVersion returned will not be greater than this.
    featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

    if (FAILED(pDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData,
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

    ComPtr<ID3D12RootSignature> rootSignature;
    ThrowIfFailed(pDevice->CreateRootSignature(0, signature->GetBufferPointer(),
                                               signature->GetBufferSize(),
                                               IID_PPV_ARGS(rootSignature.GetAddressOf())));

    return rootSignature;
}

DrawData::PerDrawPassDataObject DrawData::PerDrawPassData() const
{
    return PerDrawPassDataObject(ViewMatrix(), ProjectionMatrix());
}

XMMATRIX XM_CALLCONV DrawData::PerDrawCallData(UINT index) const
{
    return XMLoadFloat4x4(&m_modelMatrix[index]);
}

UINT DrawData::NumObjects() const
{
    return m_meshEntries.size();
}

void XM_CALLCONV DrawData::UpdatePerDrawCallData(UINT index, FXMMATRIX modelMatrix)
{
    XMStoreFloat4x4(&m_modelMatrix[index], modelMatrix);
}

void XM_CALLCONV DrawData::UpdateViewMatrix(FXMMATRIX viewMatrix)
{
    XMStoreFloat4x4(&m_viewMatrix, viewMatrix);
}

void XM_CALLCONV DrawData::UpdateProjectionMatrix(FXMMATRIX projectionMatrix)
{
    XMStoreFloat4x4(&m_projectionMatrix, projectionMatrix);
}

const std::vector<MeshEntry> &DrawData::MeshEntries() const
{
    return m_meshEntries;
}

XMMATRIX XM_CALLCONV DrawData::ViewMatrix() const
{
    return XMLoadFloat4x4(&m_viewMatrix);
}

XMMATRIX XM_CALLCONV DrawData::ProjectionMatrix() const
{
    return XMLoadFloat4x4(&m_projectionMatrix);
}

} // namespace dxultra