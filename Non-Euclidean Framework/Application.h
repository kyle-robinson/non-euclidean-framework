#pragma once
#ifndef APPLICATION_H
#define APPLICATION_H

#include <DirectXCollision.h>
#include "DDSTextureLoader.h"
#include "Resource.h"

#include "FXAA.h"
#include "Cube.h"
#include "Light.h"
#include "Input.h"
#include "Timer.h"
#include "Mapping.h"
#include "Shaders.h"
#include "MotionBlur.h"
#include "StencilCube.h"
#include "ImGuiManager.h"
#include "NonEuclidean.h"
#include "StencilCubeInv.h"
#include "PostProcessing.h"
#include "WindowContainer.h"
#include "RenderableGameObject.h"

class Application : public WindowContainer
{
public:
	bool Initialize( HINSTANCE hInstance, int width, int height );
	void CleanupDevice();

	bool ProcessMessages() noexcept;
	void Update();
	void Render();

	void SpawnControlWindows();
private:
	// Objects
	Cube m_cube;
	Face m_face;
	Light m_light;
	Camera m_camera;
	ImGuiManager m_imgui;
	RenderableGameObject m_objSkysphere;

	// Non-Euclidean Objects
	StencilCube m_stencilCube; // recursive cube
	StencilCubeInv m_stencilCubeInv; // inner room views
	std::unordered_map<Side, Camera> m_stencilCameras; // camera views
	std::vector<StencilCubeInv> m_stencilCubesInvRecursive; // room recursions

	// Systems
	FXAA m_fxaa;
	Mapping m_mapping;
	MotionBlur m_motionBlur;
	NonEuclidean m_nonEuclidean;
	PostProcessing m_postProcessing;

	// Data
	Timer m_timer;
	Input m_input;
	XMFLOAT2 m_fStencilAspect;
	float m_fStencilFov = 75.0f;
	float m_fTextureBorder = 0.05f;
	bool m_bUseStaticCamera = true;
	bool m_bUseRepeatingSpace = true;
	XMFLOAT4X4 m_previousViewProjection;
	ConstantBuffer<Matrices> m_cbMatrices;
	ConstantBuffer<TextureBorder_CB> m_cbTextureBorder;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTexture;
};

#endif