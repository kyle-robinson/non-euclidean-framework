#pragma once
#ifndef STRUCTURES_H
#define STRUCTURES_H

using namespace DirectX;

// Matrices
struct Matrices
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
};

// Materials
struct MaterialData
{
	MaterialData()
		: Emissive( 0.0f, 0.0f, 0.0f, 1.0f )
		, Ambient( 0.1f, 0.1f, 0.1f, 1.0f )
		, Diffuse( 1.0f, 1.0f, 1.0f, 1.0f )
		, Specular( 1.0f, 1.0f, 1.0f, 1.0f )
		, SpecularPower( 128.0f )
		, UseTexture( TRUE )
	{}

	XMFLOAT4 Emissive;
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;
	
	FLOAT SpecularPower;
	BOOL UseTexture;
	XMFLOAT2 Padding;
};

struct Material_CB
{
	MaterialData Material;
};

// Lights
enum LightType
{
	DirectionalLight = 0,
	PointLight = 1,
	SpotLight = 2
};

struct LightData
{
	LightData()
		: Position( 0.0f, 0.0f, 0.0f, 1.0f )
		, Direction( 0.0f, 0.0f, 1.0f, 0.0f )
		, Color( 1.0f, 1.0f, 1.0f, 1.0f )
		, SpotAngle( XM_PIDIV2 )
		, ConstantAttenuation( 1.0f )
		, LinearAttenuation( 0.0f )
		, QuadraticAttenuation( 0.0f )
		, LightType( PointLight )
		, Enabled( TRUE )
	{}

	XMFLOAT4 Position;
	XMFLOAT4 Direction;
	XMFLOAT4 Color;

	FLOAT SpotAngle;
	FLOAT ConstantAttenuation;
	FLOAT LinearAttenuation;
	FLOAT QuadraticAttenuation;

	int LightType;
	BOOL Enabled;
	XMFLOAT2 Padding;
};

#define MAX_LIGHTS 1
struct Light_CB
{
	Light_CB()
		: EyePosition( 0.0f, 0.0f, 0.0f, 1.0f )
		, GlobalAmbient( 0.2f, 0.2f, 0.8f, 1.0f )
	{}

	XMFLOAT4 EyePosition;
	XMFLOAT4 GlobalAmbient;
	LightData Lights[MAX_LIGHTS];
};

// Texture Mapping
struct MappingData
{
	MappingData()
		: UseNormalMap( TRUE )
		, UseParallaxMap( TRUE )
		, UseParallaxOcclusion( TRUE )
		, UseParallaxSelfShadowing( TRUE )
		, UseSoftShadow( TRUE )
		, HeightScale( 0.1f )
	{}

	BOOL UseNormalMap;
	BOOL UseParallaxMap;
	BOOL UseParallaxOcclusion;
	BOOL UseParallaxSelfShadowing;

	BOOL UseSoftShadow;
	FLOAT HeightScale;
	XMFLOAT2 Padding;
};

struct Mapping_CB
{
	MappingData MapData;
};

#endif