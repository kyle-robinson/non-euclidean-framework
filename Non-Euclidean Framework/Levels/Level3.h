#pragma once
#ifndef LEVEL3_H
#define LEVEL3_H

#include "Cube.h"
#include "Light.h"
#include "Mapping.h"
#include "NonEuclidean.h"
#include "LevelContainer.h"
#include "StencilCubeInv.h"

/// <summary>
/// Inherits from Level to render/update objects used in each level.
/// </summary>
class Level3 : public LevelContainer
{
public:
	Level3( const std::string& name )
	{
		m_sLevelName = name;
	}
	~Level3() {}

	void OnCreate() override;
	void OnSwitch() override;
	void BeginFrame() override {}
	void RenderFrame() override;
	void SpawnWindows() override;
	void Update( const float dt ) override;
	void CleanUp() override {}

	inline std::string GetLevelName() { return m_sLevelName; };

private:
	Cube m_cube;
	Light m_light;
	Mapping m_mapping;
	NonEuclidean m_nonEuclidean;
	StencilCubeInv m_stencilCubeInv;
	RenderableGameObject m_nanosuit;

	bool m_bUpdatePos = true;
	bool m_bDrawCube = true;
	bool m_bDrawNanosuit = false;
	float m_fTextureBorder = 0.05f;

	ConstantBuffer<Matrices> m_cbMatrices;
	ConstantBuffer<TextureBorder_CB> m_cbTextureBorder;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTexture;
};

#endif