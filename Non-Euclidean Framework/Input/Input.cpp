#include "stdafx.h"
#include "Input.h"
#include "Camera.h"

void Input::Initialize( RenderWindow& window, Camera& pCamera )
{
    m_pCamera = &pCamera;
    renderWindow = window;

    // Update keyboard processing
    keyboard.DisableAutoRepeatKeys();
    keyboard.DisableAutoRepeatChars();
}

void Input::Update( float dt )
{
    UpdateMouse( dt );
    UpdateKeyboard( dt );
    if ( m_pCamera->CanCollide() )
        UpdateCameraCollisions();
}

void Input::UpdateMouse( float dt )
{
    // update camera orientation
    while ( !mouse.EventBufferIsEmpty() )
    {
        Mouse::MouseEvent me = mouse.ReadEvent();
        if ( mouse.IsRightDown() || !cursorEnabled )
        {
            if ( me.GetType() == Mouse::MouseEvent::EventType::RawMove )
            {
                m_pCamera->AdjustRotation(
                    static_cast<float>( me.GetPosY() ) * 0.005f,
                    static_cast<float>( me.GetPosX() ) * 0.005f,
                    0.0f
                );
            }

            HideCursor();
            DisableImGuiMouse();
            m_bMovingCursor = true;
        }
        else if ( m_bMovingCursor )
        {
            ShowCursor();
            EnableImGuiMouse();
            m_bMovingCursor = false;
        }
    }
}

void Input::UpdateKeyboard( float dt )
{
    // Handle input for single key presses
	while ( !keyboard.KeyBufferIsEmpty() )
	{
		Keyboard::KeyboardEvent kbe = keyboard.ReadKey();
		unsigned char keycode = kbe.GetKeyCode();

		// Set cursor enabled/disabled
		if ( keycode == VK_HOME )
			EnableCursor();
		else if ( keycode == VK_END )
			DisableCursor();

        if ( keycode == VK_F1 )
            m_bEnableImGui = true;
        else if ( keycode == VK_F2 )
            m_bEnableImGui = false;
	}

    // Camera movement
    if ( keyboard.KeyIsPressed( 'W' ) ) m_pCamera->MoveForward( dt, TRUE );
    if ( keyboard.KeyIsPressed( 'A' ) ) m_pCamera->MoveLeft( dt, TRUE );
    if ( keyboard.KeyIsPressed( 'S' ) ) m_pCamera->MoveBackward( dt, TRUE );
    if ( keyboard.KeyIsPressed( 'D' ) ) m_pCamera->MoveRight( dt, TRUE );
    if ( keyboard.KeyIsPressed( VK_SPACE ) ) m_pCamera->MoveUp( dt );
    if ( keyboard.KeyIsPressed( VK_CONTROL ) ) m_pCamera->MoveDown( dt );
}

void Input::UpdateCameraCollisions()
{
    // x world collisions
    static float offset = 5.0f;
    if ( m_pCamera->GetPositionFloat3().x <= -offset )
        m_pCamera->SetPosition( offset, m_pCamera->GetPositionFloat3().y, m_pCamera->GetPositionFloat3().z );
    else if ( m_pCamera->GetPositionFloat3().x >= offset )
        m_pCamera->SetPosition( -offset, m_pCamera->GetPositionFloat3().y, m_pCamera->GetPositionFloat3().z );

    // y world collisions
    if ( m_pCamera->GetPositionFloat3().y <= -offset )
        m_pCamera->SetPosition( m_pCamera->GetPositionFloat3().x, offset, m_pCamera->GetPositionFloat3().z );
    else if ( m_pCamera->GetPositionFloat3().y >= offset )
        m_pCamera->SetPosition( m_pCamera->GetPositionFloat3().x, -offset, m_pCamera->GetPositionFloat3().z );

    // z world collisions
    if ( m_pCamera->GetPositionFloat3().z <= -offset )
        m_pCamera->SetPosition( m_pCamera->GetPositionFloat3().x, m_pCamera->GetPositionFloat3().y, offset );
    else if ( m_pCamera->GetPositionFloat3().z >= offset )
        m_pCamera->SetPosition( m_pCamera->GetPositionFloat3().x, m_pCamera->GetPositionFloat3().y, -offset );
}