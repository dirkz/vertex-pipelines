#pragma once

#include "stdafx.h"

namespace dxultra
{

struct MeshEntry
{
    MeshEntry() : MeshEntry(0, 0, 0) {};

    MeshEntry(UINT vertexOffset, UINT indexOffset, UINT numIndices)
        : m_vertexOffset{vertexOffset}, m_indexOffset{indexOffset}, m_numIndices{numIndices} {};

    UINT VertexOffset() const
    {
        return m_vertexOffset;
    }

    UINT IndexOffset() const
    {
        return m_indexOffset;
    }

    UINT NumIndices() const
    {
        return m_numIndices;
    }

  private:
    UINT m_vertexOffset;
    UINT m_indexOffset;
    UINT m_numIndices;
};

} // namespace dxultra
