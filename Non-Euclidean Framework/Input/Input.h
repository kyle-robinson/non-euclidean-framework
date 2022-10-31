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

private:
	void UpdateMouse( float dt );
	void UpdateKeyboard( float dt );
	void UpdateCameraCollisions();

	Camera* m_pCamera;
};

#endif