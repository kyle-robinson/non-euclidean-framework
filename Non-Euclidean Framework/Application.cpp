#include "stdafx.h"
#include "Application.h"
#include <imgui/imgui.h>

#include "Level1.h"
#include "Level2.h"
#include "Level3.h"

bool Application::Initialize( HINSTANCE hInstance, int width, int height )
{
    try
    {
        // Initialize window
        if ( !renderWindow.Initialize( &m_input, hInstance, "DirectX 11 Non-Euclidean Framework", "TutorialWindowClass", width, height ) )
		    return false;

        // Initialize graphics
        if ( !graphics.Initialize( renderWindow.GetHWND(), width, height ) )
		    return false;

        // Initialize input
        m_camera.Initialize( XMFLOAT3( 0.0f, 0.0f, -3.0f ), width, height );
        m_input.Initialize( renderWindow, m_camera );
        if ( !m_imgui.Initialize( renderWindow.GetHWND(), graphics.GetDevice(), graphics.GetContext() ) )
            return false;

        // Initialize systems
        m_postProcessing.Initialize( graphics.GetDevice() );
        HRESULT hr = m_motionBlur.Initialize( graphics.GetDevice(), graphics.GetContext() );
	    COM_ERROR_IF_FAILED( hr, "Failed to create 'motion blur' system!" );
        hr = m_fxaa.Initialize( graphics.GetDevice(), graphics.GetContext() );
	    COM_ERROR_IF_FAILED( hr, "Failed to create 'FXAA' system!" );

        // Initialize levels
        std::shared_ptr<Level1> level1 = std::make_shared<Level1>( "Non-Euclidean Cube" );
        level1->Initialize( &graphics, &m_imgui, &m_camera );
        m_pLevels.push_back( std::move( level1 ) );
        m_sLevelNames.push_back( m_stateMachine.Add( m_pLevels[0] ) );

        std::shared_ptr<Level2> level2 = std::make_shared<Level2>( "Non-Euclidean Room" );
        level2->Initialize( &graphics, &m_imgui, &m_camera );
        m_pLevels.push_back( std::move( level2 ) );
        m_sLevelNames.push_back( m_stateMachine.Add( m_pLevels[1] ) );

        std::shared_ptr<Level3> level3 = std::make_shared<Level3>( "Non-Euclidean Space" );
        level3->Initialize( &graphics, &m_imgui, &m_camera );
        m_pLevels.push_back( std::move( level3 ) );
        m_sLevelNames.push_back( m_stateMachine.Add( m_pLevels[2] ) );

        m_stateMachine.SwitchTo( "Non-Euclidean Cube" );
    }
    catch ( COMException& exception )
	{
		ErrorLogger::Log( exception );
		return false;
	}

    return true;
}

void Application::CleanupDevice()
{
#ifdef _DEBUG
    // Useful for finding dx memory leaks
    ID3D11Debug* debugDevice = nullptr;
    graphics.GetDevice()->QueryInterface( __uuidof( ID3D11Debug ), reinterpret_cast<void**>( &debugDevice ) );
    debugDevice->ReportLiveDeviceObjects( D3D11_RLDO_DETAIL );
    if ( debugDevice ) debugDevice->Release();
#endif
}

bool Application::ProcessMessages() noexcept
{
    // Process messages sent to the window
	return renderWindow.ProcessMessages();
}

void Application::Update()
{
    // Update delta time
    float dt = m_timer.GetDeltaTime(); // capped at 60 fps
    if ( dt == 0.0f ) return;

    // Update input
    m_input.Update( dt );

    // Update current level
    m_stateMachine.Update( dt );
}

void Application::Render()
{
    // Setup scene for current level
    m_stateMachine.BeginRender();

    // Render normal scene
    graphics.BeginFrame();

    // Render current level
    m_stateMachine.Render();

#pragma region POST-PROCESSING
    // Setup motion blur
    XMMATRIX viewProjInv = XMMatrixInverse( nullptr, XMMatrixTranspose( m_camera.GetViewMatrix() ) * XMMatrixTranspose( m_camera.GetProjectionMatrix() ) );
    m_motionBlur.SetViewProjInv( viewProjInv );
    XMMATRIX prevViewProj = XMLoadFloat4x4( &m_previousViewProjection );
    m_motionBlur.SetPrevViewProj( prevViewProj );
    m_motionBlur.UpdateCB();

    // Setup FXAA
    m_fxaa.UpdateCB( graphics.GetWidth(), graphics.GetHeight() );

    // Render scene to texture
    graphics.BeginRenderSceneToTexture();
    graphics.RenderSceneToTexture( m_motionBlur.GetCB(), m_fxaa.GetCB() );

    // Render imgui windows
    if ( m_input.IsCursorEnabled() )
    {
        m_imgui.BeginRender();
        m_imgui.InstructionWindow();
        m_imgui.PostProcessingWindow( &m_fxaa, &m_motionBlur );
        m_imgui.SceneWindow( graphics.GetWidth(), graphics.GetHeight(), graphics.GetRenderTarget()->GetShaderResourceView() );
        m_stateMachine.SpawnWindows();
        SpawnLevelChangerWindow();
        m_imgui.EndRender();
    }

    // Present frame
    graphics.EndFrame();

    // Store current viewProj for next render pass
    XMStoreFloat4x4( &m_previousViewProjection,
        XMMatrixTranspose( m_camera.GetViewMatrix() ) *
        XMMatrixTranspose( m_camera.GetProjectionMatrix() ) );
#pragma endregion
}

void Application::SpawnLevelChangerWindow()
{
    // Level Editor
    static bool shouldSwitchLevel = false;
    if ( ImGui::Begin( "Level Editor", FALSE, ImGuiWindowFlags_AlwaysAutoResize ) )
    {
        static int levelIndex = 0;
        ImGui::Text( "Level List" );
        if ( ImGui::BeginListBox( "##Level List", ImVec2( -FLT_MIN, m_pLevels.size() * ImGui::GetTextLineHeightWithSpacing() * 1.1f ) ) )
        {
            int index = 0;
            for ( unsigned int i = 0; i < m_pLevels.size(); i++ )
            {
                const bool isSelected = ( m_sCurrentLevelName == m_pLevels[index]->GetLevelName() );
                if ( ImGui::Selectable( m_pLevels[i]->GetLevelName().c_str(), isSelected ) )
                {
                    levelIndex = index;
                    m_sCurrentLevelName = m_pLevels[index]->GetLevelName();
                    if ( m_sCurrentLevelName == m_sLevelNames[index] )
                    {
                        shouldSwitchLevel = true;
                        break;
                    }
                }

                if ( isSelected )
                    ImGui::SetItemDefaultFocus();

                index++;
            }
            ImGui::EndListBox();
        }

        ImGui::NewLine();
        ImGui::Text( "Active Level: " );
        ImGui::SameLine();
        ImGui::TextColored( ImVec4( 1.0f, 0.0f, 0.0f, 1.0f ), m_stateMachine.GetCurrentLevel()->GetLevelName().c_str() );

        // Handle level switching
        if ( ImGui::Button( "Switch To" ) && shouldSwitchLevel )
        {
            m_stateMachine.SwitchTo( m_sCurrentLevelName );
            shouldSwitchLevel = false;
        }
    }
    ImGui::End();
}