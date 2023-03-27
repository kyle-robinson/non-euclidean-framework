#pragma pack_matrix( row_major )

// Structs
struct _NonEuclidean
{
    float CurveScale;
    bool UseHyperbolic;
    bool UseElliptic;
    float Padding;
};

// Constant Buffers
cbuffer ConstantBuffer : register( b0 )
{
    float4x4 World;
    float4x4 View;
    float4x4 Projection;
};

cbuffer NonEuclideanData : register( b1 )
{
    _NonEuclidean NonEuclidean;
}

// Functions
float4 Port( float3 ePoint )
{
    float3 p = ePoint * NonEuclidean.CurveScale;
    float d = length( p );

     // Revert to Euclidean if distance is too small
    if ( d < 0.0001f )
        return float4( p, 1.0f );

    float scale = 4.0f;
    if ( NonEuclidean.UseHyperbolic )
        return float4( p / d * sinh( d ) / scale, cosh( d ) / scale );

    if ( NonEuclidean.UseElliptic )
        return float4( p / d * sin( d ) * scale, -cos( d ) * scale );

    // Euclidean space
    return float4( p, 1.0f );
}

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
    float4 WorldPos : POSITION;
};

VS_OUTPUT VS( VS_INPUT input )
{
    VS_OUTPUT output;

    if ( NonEuclidean.UseHyperbolic || NonEuclidean.UseElliptic )
        output.Position = mul( Port( (float3)input.Position ), World );
    else
        output.Position = mul( float4( input.Position, 1.0f ), World );
    output.WorldPos = output.Position;
    output.Position = mul( output.Position, View );
    output.Position = mul( output.Position, Projection );

    // convert from model to world space
    if ( NonEuclidean.UseHyperbolic || NonEuclidean.UseElliptic )
	    output.Normal = mul( Port( input.Normal ), World ).xyz;
    else
	    output.Normal = normalize( mul( float4( input.Normal, 1.0f ), World ) );
	    //output.Normal = mul( float4( input.Normal, 1.0f ), World ).xyz;
    output.TexCoord = input.TexCoord;

    return output;
}