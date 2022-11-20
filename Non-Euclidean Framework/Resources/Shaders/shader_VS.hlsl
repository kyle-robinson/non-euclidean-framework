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
	matrix World;
	matrix View;
	matrix Projection;
}

cbuffer NonEuclideanData : register( b1 )
{
    _NonEuclidean NonEuclidean;
}

// Functions
float4 Port( float3 ePoint )
{
    float3 p = ePoint * NonEuclidean.CurveScale;
    float d = length( p );
    
    if ( d < 0.0001f ) // revert to normal even is using non-euclidean space
        return float4( p, 1.0f );
    
    float scale = 4.0f;
    if ( NonEuclidean.UseHyperbolic ) // hyperbolic
        return float4( p / d * sinh( d ) / scale, cosh( d ) / scale );
    
    if ( NonEuclidean.UseElliptic ) // elliptic
        return float4( p / d * sin( d ) * scale, -cos( d ) * scale );
    
    return float4( p, 1.0f ); // normal if not using anything
}

// Vertex Shader
struct VS_INPUT
{
    float4 Position : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD0;
    float3 Tangent : TANGENT;
    float3 Binormal : BINORMAL;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float4 WorldPosition : POSITION_W;
    float4 ViewPosition : POSITION_V;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD0;
    float3 Tangent : TANGENT;
    float3 Binormal : BINORMAL;
};

VS_OUTPUT VS( VS_INPUT input )
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    if ( NonEuclidean.UseHyperbolic || NonEuclidean.UseElliptic )
        output.Position = mul( Port( (float3)input.Position ), World );
    else
        output.Position = mul( input.Position, World );
	output.WorldPosition = output.Position;
    output.Position = mul( output.Position, View );
    output.ViewPosition = output.Position;
    output.Position = mul( output.Position, Projection );

	// convert from model to world space
    if ( NonEuclidean.UseHyperbolic || NonEuclidean.UseElliptic )
	    output.Normal = mul( Port( input.Normal ), World ).xyz;
    else
	    output.Normal = mul( float4( input.Normal, 1.0f ), World ).xyz;
	
    if ( NonEuclidean.UseHyperbolic || NonEuclidean.UseElliptic )
        output.Tangent = mul( Port( input.Tangent ), World ).xyz;
    else
        output.Tangent = mul( float4( input.Tangent, 1.0f ), World ).xyz;
	
    if ( NonEuclidean.UseHyperbolic || NonEuclidean.UseElliptic )
        output.Binormal = mul( Port( input.Binormal ), World ).xyz;
    else
        output.Binormal = mul( float4( input.Binormal, 1.0f ), World ).xyz;

	output.TexCoord = input.TexCoord;
    return output;
}