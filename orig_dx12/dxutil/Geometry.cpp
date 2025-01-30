#include "Geometry.h"

#include "VertexPipeline.h"

namespace dxultra
{

constexpr double radians360 = std::numbers::pi * 2;
constexpr double radians180 = std::numbers::pi;
constexpr double radians90 = std::numbers::pi / 2;

static double TextureNormalizeU(double u)
{
    return (u + 1.0) / 2.0;
}

Geometry::VertexBufferType Geometry::TriangleFan(double radius, double height, UINT numElements)
{
    VertexBufferType buffer{};

    double zBack = height / 2;
    double zFront = -zBack;

    double angleIncrement = radians360 / numElements;
    Vertex centerFront{0.0, 0, zFront, Colors::Red, 0.5, 0.5};
    Vertex centerBack{0.0, 0, zBack, Colors::Red, 0.5, 0.5};

    for (double angle = 0; angle < radians360; angle += angleIncrement)
    {
        double x1 = radius * cos(angle);
        double y1 = radius * sin(angle);
        double x2 = radius * cos(angle + angleIncrement);
        double y2 = radius * sin(angle + angleIncrement);

        double u1 = TextureNormalizeU(cos(angle));
        double v1 = TextureNormalizeU(sin(-angle));
        double u2 = TextureNormalizeU(cos(angle + angleIncrement));
        double v2 = TextureNormalizeU(sin(-angle - angleIncrement));

        buffer.Add(centerFront);
        buffer.Add(Vertex{x2, y2, zFront, Colors::Green, u2, v2});
        buffer.Add(Vertex{x1, y1, zFront, Colors::Blue, u1, v1});

        buffer.Add(Vertex{x1, y1, zBack, Colors::Blue, u1, v1});
        buffer.Add(Vertex{x2, y2, zBack, Colors::Green, u2, v2});
        buffer.Add(centerBack);

        buffer.Add(Vertex{x1, y1, zFront, Colors::Blue, u1, v1});
        buffer.Add(Vertex{x2, y2, zFront, Colors::Green, u2, v2});
        buffer.Add(Vertex{x1, y1, zBack, Colors::Blue, u1, v1});

        buffer.Add(Vertex{x2, y2, zFront, Colors::Green, u2, v2});
        buffer.Add(Vertex{x2, y2, zBack, Colors::Green, u2, v2});
        buffer.Add(Vertex{x1, y1, zBack, Colors::Blue, u1, v1});
    }

    return buffer;
}

Geometry::VertexBufferType Geometry::Sphere(double radius, UINT numMeridians, UINT numParallels)
{
    VertexBufferType buffer{};

    double meridianIncrement = radians360 / numMeridians;
    double parallelIncrement = radians180 / numParallels;

    const std::array<XMVECTORF32, 4> colors{Colors::Red, Colors::Green, Colors::Blue,
                                            Colors::Yellow};

    UINT colorIndex = 0;

    for (double parallel = -radians90; parallel < radians90; parallel += parallelIncrement)
    {
        double parallelFactor = cos(parallel);
        double parallelFactorAbove = cos(parallel + parallelIncrement);

        double y = radius * sin(parallel);
        double yAbove = radius * sin(parallel + parallelIncrement);

        double v = TextureNormalizeU(sin(-parallel));
        double vAbove = TextureNormalizeU(sin(-parallel - parallelIncrement));

        for (double meridian = 0; meridian < radians360; meridian += meridianIncrement)
        {
            double x1 = radius * cos(meridian) * parallelFactor;
            double z1 = radius * sin(meridian) * parallelFactor;
            double x2 = radius * cos(meridian + meridianIncrement) * parallelFactor;
            double z2 = radius * sin(meridian + meridianIncrement) * parallelFactor;

            double x1Above = radius * cos(meridian) * parallelFactorAbove;
            double z1Above = radius * sin(meridian) * parallelFactorAbove;
            double x2Above = radius * cos(meridian + meridianIncrement) * parallelFactorAbove;
            double z2Above = radius * sin(meridian + meridianIncrement) * parallelFactorAbove;

            double u1 = TextureNormalizeU(sin(meridian) * parallelFactor);
            double u2 = TextureNormalizeU(sin(meridian + meridianIncrement) * parallelFactor);

            XMVECTORF32 color = colors[colorIndex];

            Vertex leftBottom{x1, y, z1, color, u1, v};
            Vertex leftTop{x1Above, yAbove, z1Above, color, u1, vAbove};
            Vertex rightTop{x2Above, yAbove, z2Above, color, u2, vAbove};
            Vertex rightBottom{x2, y, z2, color, u2, v};

            buffer.Add(leftBottom);
            buffer.Add(leftTop);
            buffer.Add(rightTop);

            buffer.Add(leftBottom);
            buffer.Add(rightTop);
            buffer.Add(rightBottom);

            colorIndex++;
            colorIndex %= colors.size();
        }
    }

    return buffer;
}

Geometry::VertexBufferType Geometry::Tetrahedron(double edgeLength)
{
    VertexBufferType buffer{};

    double xleft = -edgeLength / 2;
    double xright = edgeLength / 2;

    // The right-angle triangle on the right side, on the xz-plane:
    // xright^2 + zdim^2 = edgeLength^2
    // zdim^2 = edgeLength^2 - xright^2
    double zdim = sqrt(edgeLength * edgeLength - xright * xright);

    double zfront = -zdim / 2;
    double zback = zdim / 2;

    double ybottom = zfront;
    double ytop = zback;

    Vertex leftBottomFront = Vertex{xleft, ybottom, zfront, Colors::Red, 0.0, 1.0};
    Vertex rightBottomFront = Vertex{xright, ybottom, zfront, Colors::Blue, 1.0, 1.0};
    Vertex middleBottomBack = Vertex{0, ybottom, zback, Colors::Green, 0.5, 1.0};
    Vertex top = Vertex{0, ytop, 0, Colors::Red, 0.5, 0.0};

    // front
    buffer.Add(leftBottomFront);
    buffer.Add(top);
    buffer.Add(rightBottomFront);

    // bottom
    buffer.Add(leftBottomFront);
    buffer.Add(rightBottomFront);
    buffer.Add(middleBottomBack);

    // left
    buffer.Add(middleBottomBack);
    buffer.Add(top);
    buffer.Add(leftBottomFront);

    // right
    buffer.Add(rightBottomFront);
    buffer.Add(top);
    buffer.Add(middleBottomBack);

    return buffer;
}

} // namespace dxultra
