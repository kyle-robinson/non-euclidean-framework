#pragma once
#ifndef LEVEL1_H
#define LEVEL1_H

#include "Geometry.h"
#include "StencilCube.h"
#include "StencilCubeInv.h"
#include "LevelContainer.h"

/// <summary>
/// Inherits from Level to render/update objects used in each level.
/// </summary>
class Level1 : public LevelContainer
{
public:
	Level1( const std::string& name )
	{
		m_sLevelName = name;
	}
	~Level1() {}

	void OnCreate() override;
	void OnSwitch() override;
	void BeginFrame() override {}
	void RenderFrame() override;
	void SpawnWindows() override;
	void Update( const float dt ) override;
	void CleanUp() override {}

	inline std::string GetLevelName() { return m_sLevelName; };

private:
	std::vector<StencilCube> m_stencilCubes;
	StencilCubeInv m_stencilCubeInv;
	float m_fTextureBorder = 0.05f;

	enum GeometryType { Cylinder, Cone, Dodecahedron, Icosahedron, Octahedron, Teapot, Count };
	std::map<GeometryType, Geometry> m_geometries;
	std::vector<std::vector<int>> m_randomNums;

	ConstantBuffer<Matrices> m_cbMatrices;
	ConstantBuffer<TextureBorder_CB> m_cbTextureBorder;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTexture;
};

#endif