#pragma once

#include "stdafx.h"

#include "IndexedVertexBuffer.h"
#include "MeshEntry.h"

namespace dxultra
{

template <class T, class I = UINT16, class Hash = std::hash<T>, class Equal = std::equal_to<T>>
struct Mesh
{
    using IndexedVertexBufferType = IndexedVertexBuffer<T, I, Hash, Equal>;

    MeshEntry Add(const IndexedVertexBufferType &buffer)
    {
        MeshEntry entry{static_cast<UINT>(m_vertices.size()), static_cast<UINT>(m_indices.size()),
                        buffer.NumIndices()};

        m_vertices.insert(m_vertices.end(), buffer.Vertices().begin(), buffer.Vertices().end());
        m_indices.insert(m_indices.end(), buffer.Indices().begin(), buffer.Indices().end());

        m_entries.push_back(entry);

        return entry;
    }

    UINT VertexBufferSizeInBytes()
    {
        return m_vertices.size() * sizeof(T);
    }

    UINT IndexBufferSizeInBytes()
    {
        return m_indices.size() * sizeof(I);
    }

    const std::vector<T> &Vertices() const
    {
        return m_vertices;
    }

    const std::vector<I> &Indices() const
    {
        return m_indices;
    }

  private:
    std::vector<T> m_vertices;
    std::vector<I> m_indices;
    std::vector<MeshEntry> m_entries;
};

} // namespace dxultra
