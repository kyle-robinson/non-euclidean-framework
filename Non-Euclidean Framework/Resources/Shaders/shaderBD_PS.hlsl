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

    // Set texture border colour
    float borderOffset = 0.05f;
    if ( input.TexCoord.x < borderOffset || input.TexCoord.y < borderOffset ||
         input.TexCoord.x > ( 1.0f - borderOffset ) || input.TexCoord.y > ( 1.0f - borderOffset ) )
        sampleColor = float3( 0.1f, 0.1f, 0.1f );

    return float4( sampleColor, 1.0f );
}