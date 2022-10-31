#pragma once
#ifndef APPLICATION_H
#define APPLICATION_H

#include <DirectXCollision.h>
#include "DDSTextureLoader.h"
#include "Resource.h"

#include "Cube.h"
#include "Input.h"
#include "Timer.h"
#include "Light.h"
#include "Mapping.h"
#include "Shaders.h"
#include "ImGuiManager.h"
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
private:
	// Objects
	Cube m_cube;
	Light m_light;
	Camera m_camera;
	Mapping m_mapping;
	ImGuiManager m_imgui;
	RenderableGameObject m_skysphere;

	// Program data/systems
	Timer m_timer;
	Input m_input;
	PostProcessing m_postProcessing;
	ConstantBuffer<Matrices> m_cbMatrices;
};

#endif