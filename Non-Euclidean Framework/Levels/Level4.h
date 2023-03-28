#pragma once
#ifndef LEVEL4_H
#define LEVEL4_H

#include "StencilCube.h"
#include "LevelContainer.h"
#include "StencilCubeInv.h"

/// <summary>
/// Inherits from Level to render/update objects used in each level.
/// </summary>
class Level4 : public LevelContainer
{
public:
	Level4( const std::string& name )
	{
		m_sLevelName = name;
	}
	~Level4() {}

	void OnCreate() override;
	void OnSwitch() override;
	void BeginFrame() override {}
	void RenderFrame() override;
	void SpawnWindows() override;
	void Update( const float dt ) override;
	void CleanUp() override {}

	inline std::string GetLevelName() { return m_sLevelName; };

private:
	float m_fRoomDepth = 5.0f;
	float m_fTextureBorder = 0.05f;
	StencilCube m_stencilCube;
	StencilCubeInv m_stencilCubeInv;
	ConstantBuffer<Matrices> m_cbMatrices;
	ConstantBuffer<TextureBorder_CB> m_cbTextureBorder;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pWallTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pArchTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pArchLongTexture;
};

#endif