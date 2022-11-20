// Structs
struct _NonEuclidean
{
    float CurveScale;
    bool UseHyperbolic;
    bool UseElliptic;
    float Padding;
};

// Constant Buffers
cbuffer NonEuclideanData : register( b0 )
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
    
    if ( NonEuclidean.UseHyperbolic ) // hyperbolic
        return float4( p / d * sin( d ), -cos( d ) );
    
    if ( NonEuclidean.UseElliptic ) // elliptic
        return float4( p / d * sinh( d ), cosh( d ) );
    
    return float4( p, 1.0f ); // normal if not using anything
}

// Vertex Shader
struct VS_INPUT
{
    float2 Position : POSITION;
    float2 TexCoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

VS_OUTPUT VS( VS_INPUT input )
{   
    VS_OUTPUT output;
    if ( NonEuclidean.UseHyperbolic || NonEuclidean.UseElliptic )
    {
        output.Position = Port( float3( input.Position, 0.0f ) ); // hyperbolic/elliptic
        
        if ( NonEuclidean.UseHyperbolic )
            output.Position *= 2.0f; // hyperbolic
        
        if ( NonEuclidean.UseElliptic )
            output.Position /= 2.0f; // elliptic
        
        output.Position.z = 0.0f;
        output.Position.w = 1.0f;
    }
    else
    {
        output.Position = float4( input.Position, 0.0f, 1.0f ); // normal
    }
    output.TexCoord = input.TexCoord;
    return output;
}