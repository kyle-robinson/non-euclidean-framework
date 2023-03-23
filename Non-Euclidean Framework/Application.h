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
#include "ImGuiManager.h"
#include "PostProcessing.h"
#include "WindowContainer.h"
#include "LevelStateMachine.h"

class Application : public WindowContainer
{
public:
	bool Initialize( HINSTANCE hInstance, int width, int height );
	void CleanupDevice();

	bool ProcessMessages() noexcept;
	void Update();
	void Render();
private:
	void SpawnLevelChangerWindow();

	// Scene
	Camera m_camera;
	ImGuiManager m_imgui;

	// Levels
	int m_iActiveLevelIdx = 0;
	bool m_bFirstLoad = false;
	LevelStateMachine m_stateMachine;
	std::vector<std::string> m_sLevelNames;
	std::string m_sCurrentLevelName = "Menu";
	std::vector<std::shared_ptr<LevelContainer>> m_pLevels;

	// Systems
	FXAA m_fxaa;
	Input m_input;
	Timer m_timer;
	MotionBlur m_motionBlur;
	PostProcessing m_postProcessing;
	XMFLOAT4X4 m_previousViewProjection;
};

#endif