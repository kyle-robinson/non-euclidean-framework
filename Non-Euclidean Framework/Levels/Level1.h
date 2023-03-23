#pragma once
#ifndef LEVEL1_H
#define LEVEL1_H

#include "Face.h"
#include "Geometry.h"
#include "StencilCube.h"
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
	void SpawnWindows() override {}
	void Update( const float dt ) override;
	void CleanUp() override {}

	inline std::string GetLevelName() { return m_sLevelName; };

private:
	Face m_face;
	StencilCube m_stencilCube;
	float m_fTextureBorder = 0.05f;

	Geometry m_cylinder;
	Geometry m_cone;
	Geometry m_dodecahedron;
	Geometry m_icosahedron;
	Geometry m_octahedron;
	Geometry m_teapot;

	ConstantBuffer<Matrices> m_cbMatrices;
	ConstantBuffer<TextureBorder_CB> m_cbTextureBorder;
};

#endif