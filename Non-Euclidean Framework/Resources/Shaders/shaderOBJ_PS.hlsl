#pragma pack_matrix( row_major )

// Resources
Texture2D textureObj : register( t0 );
SamplerState samplerState : register( s0 );

// Pixel Shader
struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
    float3 Normal : NORMAL;
};

float4 PS( PS_INPUT input ) : SV_TARGET
{
    float3 sampleColor = textureObj.Sample( samplerState, input.TexCoord );
    return float4( sampleColor, 1.0f );
}