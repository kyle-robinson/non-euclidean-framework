// Constant Buffers
cbuffer ConstantBuffer : register( b0 )
{
    float4x4 World;
    float4x4 View;
    float4x4 Projection;
};

// Vertex Shader
struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
    float4 WorldPosition : POSITION_W;
};

VS_OUTPUT VS( VS_INPUT input )
{
    VS_OUTPUT output;

    output.Position = mul( float4( input.Position, 1.0f ), World );
    output.WorldPosition = output.Position;
    output.Position = mul( output.Position, View );
    output.Position = mul( output.Position, Projection );

    output.Normal = input.Normal;
    output.TexCoord = input.TexCoord;

    return output;
}