#pragma pack_matrix( row_major )

// Definitions
#define MAX_LIGHTS 1

// Resources
Texture2D textureObj : register( t0 );
SamplerState samplerState : register( s0 );

// Structs
struct _Material
{
    float4 Emissive;
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;

    float SpecularPower;
    bool UseTexture;
    float2 Padding;
};

struct Light
{
    float4 Position;
    float4 Direction;
    float4 Color;

    float SpotAngle;
    float ConstantAttenuation;
    float LinearAttenuation;
    float QuadraticAttenuation;

    float Intensity;
    int LightType;
    bool Enabled;
    float Padding;
};

struct LightingResult
{
    float4 Diffuse;
    float4 Specular;
};

struct _NonEuclidean
{
    float CurveScale;
    bool UseHyperbolic;
    bool UseElliptic;
    float Padding;
};

// Constant Buffers
cbuffer MaterialProperties : register( b0 )
{
	_Material Material;
};

cbuffer LightProperties : register( b1 )
{
    float4 CameraPosition;
    float4 GlobalAmbient;
    Light Lights[MAX_LIGHTS];
};

cbuffer NonEuclideanData : register( b3 )
{
    _NonEuclidean NonEuclidean;
}

// Math Functions
float DotProduct( float4 u, float4 v )
{
    float curv = 0.0f;
    if ( NonEuclidean.UseHyperbolic )
        curv = -1.0f;
    if ( NonEuclidean.UseElliptic )
        curv = 1.0f;
    return dot( u, v ) - ( ( curv < 0.0f ) ? 2.0f * u.w * v.w : 0.0f );
}

float4 Direction( float4 to, float4 from )
{
    float curv = 0.0f;
    if ( NonEuclidean.UseHyperbolic )
        curv = -1.0f;
    if ( NonEuclidean.UseElliptic )
        curv = 1.0f;
    float dp = ( curv != 0.0f ) ? DotProduct( from, to ) : 1.0f;
    return to - from * dp;
}

// Lighting Functions
float4 DoDiffuse( Light light, float3 L, float3 N )
{
	float NdotL = max( 0.0f, DotProduct( float4( N, 0.0f ), float4( L, 0.0f ) ) );
	return light.Color * NdotL;
}

float4 DoSpecular( Light lightObject, float3 vertexToEye, float3 lightDirectionToVertex, float3 Normal )
{
	float3 lightDir = normalize( -lightDirectionToVertex );
	vertexToEye = normalize( vertexToEye );

	float lightIntensity = saturate( DotProduct( float4( Normal, 0.0f ), float4( lightDir, 0.0f ) ) );
	float4 specular = float4( 0.0f, 0.0f, 0.0f, 0.0f );

	if ( lightIntensity > 0.0f )
	{
		float3  reflection = normalize( 2.0f * lightIntensity * Normal - lightDir );
		specular = pow( saturate( DotProduct( float4( reflection, 0.0f ), float4( vertexToEye, 0.0f ) ) ), Material.SpecularPower );
	}

	return specular;
}

float DoAttenuation( Light light, float d )
{
	return 1.0f / ( light.ConstantAttenuation + light.LinearAttenuation * d + light.QuadraticAttenuation * d * d );
}

LightingResult DoPointLight( Light light, float3 vertexToEye, float4 vertexPos, float3 N )
{
	LightingResult result;

	float3 LightDirectionToVertex = ( vertexPos - light.Position ).xyz;
	float distance = length( LightDirectionToVertex );
	LightDirectionToVertex = LightDirectionToVertex  / distance;

	float3 vertexToLight = ( light.Position - vertexPos ).xyz;
	distance = length( vertexToLight );
	vertexToLight = vertexToLight / distance;

	float attenuation = DoAttenuation( light, distance );
	//attenuation = 1;

	result.Diffuse = DoDiffuse( light, vertexToLight, N ) * attenuation;
	result.Specular = DoSpecular( light, vertexToEye, LightDirectionToVertex, N ) * attenuation;

	return result;
}

LightingResult ComputeLighting( float4 vertexPos, float3 N, float3 vertexToEye )
{
	LightingResult totalResult = { { 0, 0, 0, 0 },{ 0, 0, 0, 0 } };

	[unroll]
	for ( int i = 0; i < MAX_LIGHTS; ++i )
	{
		LightingResult result = { { 0, 0, 0, 0 },{ 0, 0, 0, 0 } };

		if ( !Lights[i].Enabled )
			continue;

		result = DoPointLight( Lights[i], vertexToEye, vertexPos, N );

		totalResult.Diffuse += result.Diffuse;
		totalResult.Specular += result.Specular;
	}

	totalResult.Diffuse = saturate( totalResult.Diffuse );
	totalResult.Specular = saturate( totalResult.Specular );

	return totalResult;
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
    float3 vertexToLight = normalize( Direction( Lights[0].Position, input.WorldPos ) ).xyz;
    float3 vertexToEye = normalize( Direction( CameraPosition, input.WorldPos ) ).xyz;
    LightingResult lit = ComputeLighting( input.WorldPos, normalize( input.Normal ), vertexToLight );

    float power = 4.0f;
    float3 emissive = Material.Emissive.rgb * Lights[0].Intensity * power;
    float3 ambient = Material.Ambient.rgb * GlobalAmbient.rgb * Lights[0].Intensity * power;
    float3 diffuse = Material.Diffuse.rgb * lit.Diffuse.rgb * Lights[0].Intensity * power;
    float3 specular = Material.Specular.rgb * lit.Specular.rgb * Lights[0].Intensity * power;

	float3 texColor = { 1.0f, 1.0f, 1.0f };
	if ( Material.UseTexture )
        texColor = textureObj.Sample( samplerState, input.TexCoord ).rgb;

	float3 finalColor = ( emissive + ambient + diffuse + specular ) * texColor;
	//return float4( finalColor, 1.0f );
	return float4( texColor, 1.0f );
}