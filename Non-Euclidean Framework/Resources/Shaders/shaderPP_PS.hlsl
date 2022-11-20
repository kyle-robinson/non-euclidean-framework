// Resources
Texture2D textureQuad : register( t0 );
Texture2D textureDepth : register( t1 );
SamplerState samplerState : register( s0 );

// Structs
struct _MotionBlur
{
    matrix ViewProjectionInverseMatrix;
    matrix PreviousViewProjectionMatrix;

    bool UseMotionBlur;
    int NumSamples;
    float2 Padding;
};

struct _FXAA
{
    float2 TextureSizeInverse;
    float SpanMax;
    float ReduceMin;

    float ReduceMul;
    bool UseFXAA;
    float2 Padding;
};

// Constant Buffers
cbuffer MotionBlurData : register( b0 )
{
    _MotionBlur MotionBlur;
}

cbuffer FXAAData : register( b1 )
{
    _FXAA FXAA;
}

// Functions
float4 DoMotionBlur( float2 texCoord )
{
    //Get the depth buffer value at this pixel.
    float zOverW = textureDepth.Sample( samplerState, texCoord );
    // H is the viewport position at this pixel in the range -1 to 1.
    float4 H = float4( texCoord.x * 2 - 1, ( 1 - texCoord.y ) * 2 - 1, zOverW, 1 );
    // Transform by the view-projection inverse.
    float4 D = mul( H, MotionBlur.ViewProjectionInverseMatrix );
    // Divide by w to get the world position.
    float4 worldPos = D / D.w;

    // Current viewport position
    float4 currentPos = H;
    // Use the world position, and transform by the previous view-    // projection matrix.
    float4 previousPos = mul( worldPos, MotionBlur.PreviousViewProjectionMatrix );
    // Convert to nonhomogeneous points [-1,1] by dividing by w.
    previousPos /= previousPos.w;
    // Use this frame's position and last frame's to compute the pixel    // velocity.
    float2 velocity = ( currentPos - previousPos ) / 8.f;

    // Get the initial color at this pixel.
    float4 color = textureQuad.Sample( samplerState, texCoord );
    texCoord += velocity;

    for ( int i = 1; i < MotionBlur.NumSamples; ++i )
    {
        texCoord += velocity;

        // Sample the color buffer along the velocity vector.
        float4 currentColor = textureQuad.Sample( samplerState, texCoord );

        // Add the current color to our color sum.
        color += currentColor;
    }

    // Average all of the samples to get the final blur color.
    return color / MotionBlur.NumSamples;
}

float4 DoFXAA( float2 texCoord )
{
    float3 luma = { 0.299f, 0.587f, 0.114f };
    float lumaTL = dot( luma, textureQuad.Sample( samplerState, texCoord + ( float2( -1.0f,  1.0f ) * FXAA.TextureSizeInverse ) ) );
    float lumaTR = dot( luma, textureQuad.Sample( samplerState, texCoord + ( float2(  1.0f,  1.0f ) * FXAA.TextureSizeInverse ) ) );
    float lumaBL = dot( luma, textureQuad.Sample( samplerState, texCoord + ( float2( -1.0f, -1.0f ) * FXAA.TextureSizeInverse ) ) );
    float lumaBR = dot( luma, textureQuad.Sample( samplerState, texCoord + ( float2(  1.0f, -1.0f ) * FXAA.TextureSizeInverse ) ) );
    float lumaM = dot( luma, textureQuad.Sample( samplerState, texCoord ) );

    float2 dir;
	dir.x = ( ( lumaTL + lumaTR ) - ( lumaBL + lumaBR ) );
	dir.y = ( ( lumaTL + lumaBL ) - ( lumaTR + lumaBR ) );

    float dirReduce = max( ( lumaTL + lumaTR + lumaBL + lumaBR ) * ( FXAA.ReduceMul * 0.25f ), FXAA.ReduceMin );
	float inverseDirAdjustment = 1.0f / ( min( abs( dir.x ), abs( dir.y ) ) + dirReduce );

	dir.x = min( float2( FXAA.SpanMax, FXAA.SpanMax ), max( float2( -FXAA.SpanMax, -FXAA.SpanMax ), dir * inverseDirAdjustment ) ) * FXAA.TextureSizeInverse;
	dir.y = min( float2( FXAA.SpanMax, FXAA.SpanMax ), max( float2( -FXAA.SpanMax, -FXAA.SpanMax ), dir * inverseDirAdjustment ) ) * FXAA.TextureSizeInverse;

	float3 result1 = ( 1.0f / 2.0f ) * (
		textureQuad.Sample( samplerState, texCoord + ( dir * float2( 1.0f / 3.0f - 0.5f, 1.0f / 3.0f - 0.5f ) ) ) +
		textureQuad.Sample( samplerState, texCoord + ( dir * float2( 2.0f / 3.0f - 0.5f, 2.0f / 3.0f - 0.5f ) ) ) );

	float3 result2 = result1 * ( 1.0f / 2.0f ) + ( 1.0f / 4.0f ) * (
		textureQuad.Sample( samplerState, texCoord + ( dir * float2( 0.0f / 3.0f - 0.5f, 0.0f / 3.0f - 0.5f ) ) ) +
		textureQuad.Sample( samplerState, texCoord + ( dir * float2( 3.0f / 3.0f - 0.5f, 3.0f / 3.0f - 0.5f ) ) ) );

	float lumaMin = min( lumaM, min( min( lumaTL, lumaTR ), min( lumaBL, lumaBR ) ) );
	float lumaMax = max( lumaM, max( max( lumaTL, lumaTR ), max( lumaBL, lumaBR ) ) );
	float lumaResult2 = dot( luma, result2 );
	
	if ( lumaResult2 < lumaMin || lumaResult2 > lumaMax )
		return float4( result1.x, result1.y, result1.z, 1.0f );
	else
		return float4( result2.x, result2.y, result2.z, 1.0f );
}

// Pixel Shader
struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

float4 PS( PS_INPUT input ) : SV_TARGET
{
    if ( MotionBlur.UseMotionBlur )
        return DoMotionBlur( input.TexCoord );

    if ( FXAA.UseFXAA )
        return DoFXAA( input.TexCoord );

    // Sample from render target texture
    return saturate( textureQuad.Sample( samplerState, input.TexCoord ).rgba );
}