#pragma once

#include "stdafx.h"

namespace dxultra
{
struct Vertex
{
    XMFLOAT3 Position;
    XMFLOAT4 Color;
    XMFLOAT2 TexCoord;

    Vertex(float x, float y, float z, XMVECTORF32 color, float u, float v)
        : Position{x, y, z}, Color{color}, TexCoord{u, v}
    {
    }

    Vertex(double x, double y, double z, XMVECTORF32 color, double u, double v)
        : Vertex{static_cast<float>(x), static_cast<float>(y), static_cast<float>(z), color,
                 static_cast<float>(u), static_cast<float>(v)}
    {
    }

    /// <summary>
    /// Function object for vertex equality.
    /// </summary>
    struct Equal
    {
        bool operator()(const Vertex &v1, const Vertex &v2) const;
    };

    /// <summary>
    /// Function object for vertex hashing.
    /// </summary>
    struct Hash
    {
        std::size_t operator()(const Vertex &v) const;
    };

    inline std::array<float, 9> Values() const
    {
        return std::array<float, 9>{Position.x, Position.y, Position.z, Color.x,   Color.y,
                                    Color.z,    Color.w,    TexCoord.x, TexCoord.y};
    }
};

} // namespace dxultra
