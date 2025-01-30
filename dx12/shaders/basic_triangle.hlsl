cbuffer CBPerObject : register(b0)
{
    matrix g_model;
};

cbuffer CBPerPass : register(b1)
{
    matrix g_viewProjection;
};

struct Vertex
{
    float3 Position : POSITION;
    float4 Color : COLOR;
};

struct Pixel
{
    float4 Position : SV_Position;
    float4 Color : COLOR;
};

Pixel VS(Vertex v)
{
    Pixel p;

    matrix modelViewProjection = mul(g_model, g_viewProjection);
    
    p.Position = mul(float4(v.Position, 1.f), modelViewProjection);
    p.Color = v.Color;
    
    return p;
}

float4 PS(Pixel p) : SV_Target
{
    return p.Color;
}