#pragma once
#ifndef LEVELCONTAINER_H
#define LEVELCONTAINER_H

#include "Camera.h"
#include "Graphics.h"
#include "ImGuiManager.h"

/// <summary>
/// Loads and renders/updates all the components and models for the current scene/level.
/// Sets up any constant buffers that are specific to this particular scene/level.
/// Used to initialize objects native to every level.
/// </summary>
class LevelContainer
{
	friend class Application;
public:
	virtual ~LevelContainer( void ) = default;
	inline void Initialize( Graphics* gfx, ImGuiManager* imgui, Camera* cam )
	{
		m_gfx = gfx;
		m_camera = cam;
		m_imgui = imgui;
	}

	inline bool GetIsCreated() const noexcept { return m_bIsCreated; }
	inline void SetIsCreated( bool created ) noexcept { m_bIsCreated = created; }

	inline void SetLevelName( std::string name ) noexcept { m_sLevelName = name; }
	inline std::string GetLevelName() const noexcept { return m_sLevelName; }

	inline Camera* GetCamera() const noexcept { return m_camera; }
	inline Graphics* GetGraphics() const noexcept { return m_gfx; }
	inline ImGuiManager* GetImguiManager() const noexcept { return m_imgui; }

	// Render/Update Scene Functions
	virtual void OnCreate() {}
	virtual void OnSwitch() {}
	virtual void BeginFrame() {}
	virtual void RenderFrame() {}
	virtual void SpawnWindows() {}
	virtual void Update( const float dt ) {}
	virtual void CleanUp() {}

protected:
	// Objects
	Camera* m_camera;
	Graphics* m_gfx;
	ImGuiManager* m_imgui;

	// Next Level
	bool m_bIsCreated = false;
	std::string m_sLevelName;
	int m_iCurrentLevel;
	int m_iNextLevel;
};

#endif