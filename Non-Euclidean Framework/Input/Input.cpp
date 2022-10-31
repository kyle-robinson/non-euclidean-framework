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
	}

    // Normalize diagonal movement speed
	if ( keyboard.KeyIsPressed( 'W' ) && ( keyboard.KeyIsPressed( 'A' ) || keyboard.KeyIsPressed( 'S' ) ) )
		m_pCamera->SetCameraSpeed( 2.0f );
	if ( keyboard.KeyIsPressed( 'S' ) && ( keyboard.KeyIsPressed( 'A' ) || keyboard.KeyIsPressed( 'S' ) ) )
        m_pCamera->SetCameraSpeed( 2.0f );

	if ( keyboard.KeyIsPressed( 'W' ) && ( keyboard.KeyIsPressed( 'D' ) || keyboard.KeyIsPressed( 'S' ) ) )
        m_pCamera->SetCameraSpeed( 2.0f );
	if ( keyboard.KeyIsPressed( 'S' ) && ( keyboard.KeyIsPressed( 'D' ) || keyboard.KeyIsPressed( 'S' ) ) )
        m_pCamera->SetCameraSpeed( 2.0f );

    // Camera movement
    if ( keyboard.KeyIsPressed( 'W' ) ) m_pCamera->MoveForward( dt );
    if ( keyboard.KeyIsPressed( 'A' ) ) m_pCamera->MoveLeft( dt );
    if ( keyboard.KeyIsPressed( 'S' ) ) m_pCamera->MoveBackward( dt );
    if ( keyboard.KeyIsPressed( 'D' ) ) m_pCamera->MoveRight( dt );

    // Camera speed
    m_pCamera->SetCameraSpeed( 2.5f );
}

void Input::UpdateCameraCollisions()
{
    // x world collisions
    if ( m_pCamera->GetPositionFloat3().x <= -5.0f )
        m_pCamera->SetPosition( -5.0f, m_pCamera->GetPositionFloat3().y, m_pCamera->GetPositionFloat3().z );
    if ( m_pCamera->GetPositionFloat3().x >= 5.0f )
        m_pCamera->SetPosition( 5.0f, m_pCamera->GetPositionFloat3().y, m_pCamera->GetPositionFloat3().z );

    // y world collisions
    if ( m_pCamera->GetPositionFloat3().y <= -5.0f )
        m_pCamera->SetPosition( m_pCamera->GetPositionFloat3().x, -5.0f, m_pCamera->GetPositionFloat3().z );
    if ( m_pCamera->GetPositionFloat3().y >= 5.0f )
        m_pCamera->SetPosition( m_pCamera->GetPositionFloat3().x, 5.0f, m_pCamera->GetPositionFloat3().z );

    // z world collisions
    if ( m_pCamera->GetPositionFloat3().z <= -5.0f )
        m_pCamera->SetPosition( m_pCamera->GetPositionFloat3().x, m_pCamera->GetPositionFloat3().y, -5.0f );
    if ( m_pCamera->GetPositionFloat3().z >= 5.0f )
        m_pCamera->SetPosition( m_pCamera->GetPositionFloat3().x, m_pCamera->GetPositionFloat3().y, 5.0f );
}