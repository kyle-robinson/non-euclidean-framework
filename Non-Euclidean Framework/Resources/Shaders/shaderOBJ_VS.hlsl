#pragma pack_matrix( row_major )

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
    float2 TexCoord : TEXCOORD;
    float3 Normal : NORMAL;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
    float3 Normal : NORMAL;
};

VS_OUTPUT VS( VS_INPUT input )
{
    VS_OUTPUT output;
        
    output.Position = mul( float4( input.Position, 1.0f ), World );
    output.Position = mul( output.Position, View );
    output.Position = mul( output.Position, Projection );

    output.Normal = normalize( mul( float4( input.Normal, 1.0f ), World ) );
    output.TexCoord = input.TexCoord;
    
    return output;
}