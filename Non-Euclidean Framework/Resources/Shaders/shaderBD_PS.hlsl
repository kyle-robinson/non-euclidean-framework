#pragma pack_matrix( row_major )

// Resources
Texture2D textureObj : register( t0 );
SamplerState samplerState : register( s0 );

// Constant Buffers
cbuffer TextureBorder_CB : register( b0 )
{
    float TextureBorder;
    float3 Padding;
}

// Pixel Shader
struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
    float3 Normal : NORMAL;
    float4 WorldPos : POSITION;
};

float4 PS( PS_INPUT input ) : SV_TARGET
{
    float3 sampleColor = textureObj.Sample( samplerState, input.TexCoord );

    // Set texture border colour
    if ( input.TexCoord.x < TextureBorder || input.TexCoord.y < TextureBorder ||
         input.TexCoord.x > ( 1.0f - TextureBorder ) || input.TexCoord.y > ( 1.0f - TextureBorder ) )
        sampleColor = float3( 0.1f, 0.1f, 0.1f );

    return float4( sampleColor, 1.0f );
}