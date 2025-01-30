#pragma once

#include "stdafx.h"

namespace dxultra
{

template <class T, class I = UINT16, class Hash = std::hash<T>, class Equal = std::equal_to<T>>
struct IndexedVertexBuffer
{
    IndexedVertexBuffer()
    {
    }

    void Add(const T &e)
    {
        auto existing = m_map.find(e);
        if (existing == m_map.end())
        {
            I index = static_cast<I>(m_vertices.size());
            m_vertices.push_back(e);
            m_map[e] = index;
            m_indices.push_back(index);
        }
        else
        {
            I index = m_map[e];
            m_indices.push_back(index);
        }
    }

    UINT VertexBufferSizeInBytes() const
    {
        return m_vertices.size() * sizeof(T);
    }

    UINT IndexBufferSizeInBytes() const
    {
        return m_indices.size() * sizeof(I);
    }

    UINT NumIndices() const
    {
        return m_indices.size();
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
    std::unordered_map<T, I, Hash, Equal> m_map;
};

} // namespace dxultra
