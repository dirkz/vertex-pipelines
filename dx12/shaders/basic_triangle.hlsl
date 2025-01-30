cbuffer CBPerPass : register(b0)
{
    matrix g_view;
    matrix g_projection;
};

cbuffer CBPerObject : register(b1)
{
    matrix g_model;
};

struct Vertex
{
    float3 Position : POSITION;
    float4 Color : COLOR;
    float2 TexCoord : TEXCOORD;
};

struct Pixel
{
    float4 Position : SV_Position;
    float4 Color : COLOR;
    float2 TexCoord : TEXCOORD;
};

Pixel VS(Vertex v)
{
    Pixel p;

    matrix viewProjection = mul(g_view, g_projection);
    matrix modelViewProjection = mul(g_model, viewProjection);
    
    p.Position = mul(float4(v.Position, 1.f), modelViewProjection);
    p.Color = v.Color;
    p.TexCoord = v.TexCoord;
    
    return p;
}

float4 PS(Pixel p) : SV_Target
{
    return float4(0, p.TexCoord.x, p.TexCoord.y, 1);
}