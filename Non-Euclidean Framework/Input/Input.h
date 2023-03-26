#pragma once
#ifndef INPUT_H
#define INPUT_H

#include "Camera.h"
#include "WindowContainer.h"

class Input : public WindowContainer
{
public:
	void Initialize( RenderWindow& window, Camera& pCamera );
	void Update( float dt );
	inline bool IsImGuiEnabled() const noexcept { return m_bEnableImGui; }

private:
	void UpdateMouse( float dt );
	void UpdateKeyboard( float dt );
	void UpdateCameraCollisions();

	Camera* m_pCamera;
	bool m_bEnableImGui = true;
	bool m_bMovingCursor = false;
};

#endif