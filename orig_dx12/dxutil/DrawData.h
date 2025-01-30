#pragma once

#include "stdafx.h"

#include "MeshEntry.h"
#include "VertexPipeline.h"

namespace dxultra
{

struct DrawData
{
    struct PerDrawPassDataObject
    {
        PerDrawPassDataObject(CXMMATRIX viewMatrix, CXMMATRIX projectionMatrix);

        XMFLOAT4X4 ViewMatrix;
        XMFLOAT4X4 ProjectionMatrix;
    };

    static ComPtr<ID3D12RootSignature> CreateRootSignature(ID3D12Device *pDevice);

    DrawData(const std::vector<MeshEntry> &meshEntries)
        : m_modelMatrix(meshEntries.size()), m_viewMatrix{}, m_projectionMatrix{},
          m_meshEntries{meshEntries}
    {
    }

    PerDrawPassDataObject PerDrawPassData() const;
    XMMATRIX XM_CALLCONV PerDrawCallData(UINT index) const;

    UINT NumObjects() const;

    void XM_CALLCONV UpdatePerDrawCallData(UINT index, FXMMATRIX modelMatrix);

    /// <summary>
    /// Update part of the per-draw-call data (convenience method).
    /// </summary>
    /// <param name="viewMatrix"></param>
    /// <returns></returns>
    void XM_CALLCONV UpdateViewMatrix(FXMMATRIX viewMatrix);

    /// <summary>
    /// Update part of the per-draw-call data (convenience method).
    /// </summary>
    /// <param name="projectionMatrix"></param>
    /// <returns></returns>
    void XM_CALLCONV UpdateProjectionMatrix(FXMMATRIX projectionMatrix);

    const std::vector<MeshEntry> &MeshEntries() const;

  private:
    XMMATRIX XM_CALLCONV ViewMatrix() const;
    XMMATRIX XM_CALLCONV ProjectionMatrix() const;

    std::vector<MeshEntry> m_meshEntries;
    std::vector<XMFLOAT4X4> m_modelMatrix;
    XMFLOAT4X4 m_viewMatrix;
    XMFLOAT4X4 m_projectionMatrix;
};

} // namespace dxultra
