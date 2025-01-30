#pragma once

#include "stdafx.h"

#include "Shader.h"

namespace dxultra
{

struct VertexPipeline
{
    VertexPipeline(ID3D12Device *pDevice);

    ID3DBlob *VertexShader();
    ID3DBlob *PixelShader();
    D3D12_INPUT_LAYOUT_DESC InputLayout();

  private:
    Shader m_vertexShader;
    Shader m_pixelShader;
};

} // namespace dxultra
