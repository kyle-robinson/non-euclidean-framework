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

#include <dxtk/GeometricPrimitive.h>
#include <dxtk/Effects.h>

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
	float m_fStencilFov = 90.0f;
	float m_fTextureBorder = 0.05f;
	XMFLOAT4X4 m_previousViewProjection;

	bool m_bDebugCubes = true;
	bool m_bUseStaticCamera = true;
	bool m_bUseRepeatingSpace = true;
	bool m_bDrawCubeSkysphere = false;
	bool m_bDrawCubeRecursion = false;

	// Constant Buffers
	ConstantBuffer<Matrices> m_cbMatrices;
	ConstantBuffer<TextureBorder_CB> m_cbTextureBorder;

	// Textures
	enum class Color
	{
		Red,
		Orange,
		Yellow,
		Green,
		Blue,
		Purple,
		Count
	};
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTexture;
	std::map<Color, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_pColorTextures;

	// Toolkit Objects
	enum class Object
	{
		Cylinder,
		Cone,
		Dodecahedron,
		Icosahedron,
		Octahedron,
		Teapot,
		Count
	};
	std::map<Object,
		std::tuple<std::unique_ptr<BasicEffect>,
		std::unique_ptr<GeometricPrimitive>,
		Microsoft::WRL::ComPtr<ID3D11InputLayout>>> m_pObjects;
};

#endif