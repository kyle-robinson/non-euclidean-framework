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
	
	inline void EnableCollisions() noexcept { m_bCollisions = true; }
	inline void DisableCollisions() noexcept { m_bCollisions = false; }
	inline bool IsCollisionsActive() const noexcept { return m_bCollisions; }

private:
	void UpdateMouse( float dt );
	void UpdateKeyboard( float dt );
	void UpdateCameraCollisions();

	Camera* m_pCamera;
	bool m_bCollisions = true;
};

#endif