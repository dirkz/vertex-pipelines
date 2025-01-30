#pragma once

#include "stdafx.h"

namespace zdx
{

struct Vertex
{
    static D3D12_INPUT_LAYOUT_DESC InputLayout();

    XMFLOAT3 Position;
    XMFLOAT4 Color;

    Vertex(float x, float y, float z, XMVECTORF32 color) : Position{x, y, z}, Color{color} {};
};

} // namespace zdx
