#pragma once
#ifndef APPLICATION_H
#define APPLICATION_H

#include <DirectXCollision.h>
#include "DDSTextureLoader.h"
#include "Resource.h"

#include "FXAA.h"
#include "Input.h"
#include "Timer.h"
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

	void SpawnControlWindow();
private:
	// Objects
	Face m_face;
	Camera m_camera;
	ImGuiManager m_imgui;
	StencilCube m_stencilCube;
	StencilCubeInv m_stencilCubeInv;
	RenderableGameObject m_objSkysphere;
	StencilCubeInv m_stencilCubeInvRecursive;
	std::unordered_map<Side, Camera> m_stencilCameras;

	// Systems
	FXAA m_fxaa;
	MotionBlur m_motionBlur;
	NonEuclidean m_nonEuclidean;
	PostProcessing m_postProcessing;

	// Data
	Timer m_timer;
	Input m_input;
	bool m_bUseStaticCamera = true;
	XMFLOAT4X4 m_previousViewProjection;
	ConstantBuffer<Matrices> m_cbMatrices;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTexture;
};

#endif