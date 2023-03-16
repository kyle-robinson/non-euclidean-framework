// Resources
Texture2D txDiffuse : register( t0 );
SamplerState samplerState : register( s0 );

// Pixel Shader
struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
    float4 WorldPosition : POSITION_W;
};

float4 PS( PS_INPUT input ) : SV_TARGET
{
    float4 textureColor = txDiffuse.Sample( samplerState, input.TexCoord );
    return textureColor;
}