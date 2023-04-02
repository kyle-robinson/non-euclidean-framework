#pragma once
#ifndef LEVEL2_H
#define LEVEL2_H

#include "Face.h"
#include "Geometry.h"
#include "StencilCube.h"
#include "StencilCubeInv.h"
#include "LevelContainer.h"
#if _x64
#include "RenderableGameObject.h"
#endif

/// <summary>
/// Inherits from Level to render/update objects used in each level.
/// </summary>
class Level2 : public LevelContainer
{
public:
	Level2( const std::string& name )
	{
		m_sLevelName = name;
	}
	~Level2() {}

	void OnCreate() override;
	void OnSwitch() override;
	void BeginFrame() override;
	void RenderFrame() override;
	void SpawnWindows() override;
	void Update( const float dt ) override;
	void CleanUp() override {}

	inline std::string GetLevelName() { return m_sLevelName; };

private:
	// Objects
	Face m_face;
	StencilCube m_stencilCube; // recursive cube
	StencilCubeInv m_stencilCubeInv; // inner room views
	std::unordered_map<Side, Camera> m_stencilCameras; // camera views
	std::vector<StencilCubeInv> m_stencilCubesInvRecursive; // room recursions
#if _x64
	RenderableGameObject m_objSkysphere;
#endif

	// Scene Data
	XMFLOAT2 m_fStencilAspect;
	float m_fStencilFov = 90.0f;
	float m_fTextureBorder = 0.05f;

	bool m_bRTTRoom = true;
	bool m_bStencilRoom = false;
	bool m_bStencilRTTRoom = false;
	bool m_bUpdateDepth = false;
	bool m_bUpdateRoom = false;

	// Constant Buffers
	ConstantBuffer<Matrices> m_cbMatrices;
	ConstantBuffer<TextureBorder_CB> m_cbTextureBorder;

	// Textures
	enum class Color { Red, Orange, Yellow, Green, Blue, Purple, Count };
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTexture;
	std::map<Color, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_pColorTextures;
};

#endif