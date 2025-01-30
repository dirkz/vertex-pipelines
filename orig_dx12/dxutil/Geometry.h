#pragma once

#include "stdafx.h"

#include "IndexedVertexBuffer.h"
#include "Mesh.h"
#include "Vertex.h"
#include "VertexPipeline.h"

namespace dxultra
{

/// <summary>
/// Methods for creating geometry, based on a left-handed coordinate system.
/// </summary>
struct Geometry
{
    using MeshType = Mesh<Vertex, UINT32, Vertex::Hash, Vertex::Equal>;
    using VertexBufferType = MeshType::IndexedVertexBufferType;

    /// <summary>
    /// Create a triangle fan in the xy-plane.
    /// </summary>
    /// <param name="radius"></param>
    /// <param name="height"></param>
    /// <param name="numElements"></param>
    /// <returns></returns>
    static VertexBufferType TriangleFan(double radius, double height, UINT numElements);

    /// <summary>
    /// Create a sphere, parallels are parallel to the xz-plane, that is, the equator circle
    /// lies on the xz-plane.
    /// </summary>
    /// <param name="radius"></param>
    /// <param name="numMeridians">The number of meridians (the lines of constant longitude running
    /// from pole to pole)</param>
    /// <param name="numParallels">The number of latitudes (the circles parallel to the
    /// equator)</param>
    /// <returns></returns>
    static VertexBufferType Sphere(double radius, UINT numMeridians, UINT numParallels);

    static VertexBufferType Tetrahedron(double edgeLength);
};

} // namespace dxultra
