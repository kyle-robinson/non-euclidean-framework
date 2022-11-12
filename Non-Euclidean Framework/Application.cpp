#include "stdafx.h"
#include "Application.h"
#include "ThreadManager.h"
#include <imgui/imgui.h>

extern uint32_t RENDER_DEPTH;
extern uint32_t CAMERA_COUNT;
extern uint32_t THREAD_COUNT;

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
        m_imgui.Initialize( renderWindow.GetHWND(), graphics.GetDevice(), graphics.GetContext() );

        for ( uint32_t i = 0; i < CAMERA_COUNT; i++ )
        {
            Camera camera;
            camera.Initialize( XMFLOAT3( 0.0f, 0.0f, 0.0f ), width, height );
            m_stencilCameras.emplace( (Side)i, std::move( camera ) );
        }
        m_stencilCameras.at( Side::FRONT ).SetPosition( XMFLOAT3( 0.0f, 0.0f, -5.0f ) );
        
        m_stencilCameras.at( Side::BACK ).SetPosition( XMFLOAT3( 0.0f, 0.0f, 5.0f ) );
        m_stencilCameras.at( Side::BACK ).SetRotation( XMFLOAT3( 0.0f, -XM_PI, 0.0f ) );
        
        m_stencilCameras.at( Side::LEFT ).SetPosition( XMFLOAT3( 5.0f, 0.0f, 0.0f ) );
        m_stencilCameras.at( Side::LEFT ).SetRotation( XMFLOAT3( 0.0f, -XM_PIDIV2, 0.0f ) );
        
        m_stencilCameras.at( Side::RIGHT ).SetPosition( XMFLOAT3( -5.0f, 0.0f, 0.0f ) );
        m_stencilCameras.at( Side::RIGHT ).SetRotation( XMFLOAT3( 0.0f, XM_PIDIV2, 0.0f ) );
        
        m_stencilCameras.at( Side::TOP ).SetPosition( XMFLOAT3( 0.0f, 5.0f, 0.0f ) );
        m_stencilCameras.at( Side::TOP ).SetRotation( XMFLOAT3( XM_PIDIV2, 0.0f, 0.0f ) );
        
        m_stencilCameras.at( Side::BOTTOM ).SetPosition( XMFLOAT3( 0.0f, -5.0f, 0.0f ) );
        m_stencilCameras.at( Side::BOTTOM ).SetRotation( XMFLOAT3( -XM_PIDIV2, 0.0f, 0.0f ) );

        // Initialize constant buffers
        HRESULT hr = m_cbMatrices.Initialize( graphics.GetDevice(), graphics.GetContext() );
	    COM_ERROR_IF_FAILED( hr, "Failed to create 'Matrices' constant buffer!" );

        // Initialize game objects
        hr = m_face.Initialize( graphics.GetContext(), graphics.GetDevice() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'face' object!" );

        hr = m_stencilCube.Initialize( graphics.GetContext(), graphics.GetDevice() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'stencil cube' object!" );

        hr = m_stencilCubeInv.Initialize( graphics.GetContext(), graphics.GetDevice() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'stencil cube inverse' object!" );

        hr = m_light.Initialize( graphics.GetDevice(), graphics.GetContext(), m_cbMatrices );
	    COM_ERROR_IF_FAILED( hr, "Failed to create 'light' object!" );

        // Initialize systems
        m_postProcessing.Initialize( graphics.GetDevice() );

        hr = m_motionBlur.Initialize( graphics.GetDevice(), graphics.GetContext() );
	    COM_ERROR_IF_FAILED( hr, "Failed to create 'motion blur' system!" );

        hr = m_fxaa.Initialize( graphics.GetDevice(), graphics.GetContext() );
	    COM_ERROR_IF_FAILED( hr, "Failed to create 'FXAA' system!" );

        // Initialize models
        if ( !m_objSkysphere.Initialize( "Resources\\Models\\sphere.obj", graphics.GetDevice(), graphics.GetContext(), m_cbMatrices ) )
		    return false;
        m_objSkysphere.SetInitialScale( 50.0f, 50.0f, 50.0f );

        // Initialize Textures
        hr = CreateDDSTextureFromFile( graphics.GetDevice(), L"Resources\\Textures\\bricks_TEX.dds", nullptr, m_pTexture.GetAddressOf() );
		COM_ERROR_IF_FAILED( hr, "Failed to create 'diffuse' texture!" );
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
    // Usefult for finding dx memory leaks
    ID3D11Debug* debugDevice = nullptr;
    graphics.GetDevice()->QueryInterface( __uuidof(ID3D11Debug), reinterpret_cast<void**>( &debugDevice ) );
    debugDevice->ReportLiveDeviceObjects( D3D11_RLDO_DETAIL );
    if ( debugDevice ) debugDevice->Release();
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

    // Update skysphere position
    m_objSkysphere.SetPosition( m_camera.GetPositionFloat3() );
}

void Application::Render()
{
#pragma region RTT_CUBE
    std::function<void( uint32_t i, uint32_t j )> RenderCubeStencils = std::function( [&]( uint32_t i, uint32_t j ) -> void
    {
        // Render RTT cube
        graphics.BeginFrameCube( (Side)i, j );
        Camera camera = m_bUseStaticCamera ? m_stencilCameras.at( (Side)i ) : m_camera;

        graphics.UpdateRenderStateSkysphere();
        m_objSkysphere.Draw( camera.GetViewMatrix(), camera.GetProjectionMatrix() );
    
        // Update constant buffers
        m_light.UpdateCB( camera );

        // Draw face with stencil view
        graphics.UpdateRenderStateObject();
        if ( j > 0 )
            graphics.GetContext()->PSSetShaderResources( 0u, 1u, graphics.GetCubeBuffer( (Side)i, j - 1u )->GetShaderResourceViewPtr() );
        else if ( j == 0 )
            graphics.GetContext()->PSSetShaderResources( 0u, 1u, m_pTexture.GetAddressOf() );

        // Rotate face to point in direction of camera
        switch ( (Side)i )
        {
        case Side::FRONT: m_face.SetRotation( XMFLOAT3( 0.0f, 0.0f, 0.0f ) ); break;
        case Side::BACK: m_face.SetRotation( XMFLOAT3( 0.0f, XM_PI, 0.0f ) ); break;
        case Side::LEFT: m_face.SetRotation( XMFLOAT3( 0.0f, -XM_PIDIV2, 0.0f ) ); break;
        case Side::RIGHT: m_face.SetRotation( XMFLOAT3( 0.0f, XM_PIDIV2, 0.0f ) ); break;
        case Side::TOP: m_face.SetRotation( XMFLOAT3( XM_PIDIV2, 0.0f, 0.0f ) ); break;
        case Side::BOTTOM: m_face.SetRotation( XMFLOAT3( -XM_PIDIV2, 0.0f, 0.0f ) ); break;
        }
        m_face.Draw( m_cbMatrices, camera );

        // Draw light object
        graphics.UpdateRenderStateTexture();
        m_light.Draw( camera.GetViewMatrix(), camera.GetProjectionMatrix() );
    } );
#pragma endregion

#pragma region RTT_CUBE_INV
    std::function<void( uint32_t i, uint32_t j )> RenderCubeInvStencils = std::function( [&]( uint32_t i, uint32_t j ) -> void
    {
        // Adjust side value to account for inverse cube
        uint32_t i_inv = i;
        if ( i % 2u == 0u )
            i_inv = i + 1u;
        else
            i_inv = i - 1u;

        // Render RTT room face
        graphics.BeginFrameCubeInv( (Side)i_inv, j );
        Camera camera = m_bUseStaticCamera ? m_stencilCameras.at( (Side)i_inv ) : m_camera;

        graphics.UpdateRenderStateSkysphere();
        m_objSkysphere.Draw( camera.GetViewMatrix(), camera.GetProjectionMatrix() );

        // Render RTT cube
        graphics.UpdateRenderStateObject();
        for ( uint32_t x = 0u; x < CAMERA_COUNT; x++ )
            m_stencilCube.SetTexture( (Side)x, graphics.GetCubeBuffer( (Side)x, RENDER_DEPTH - 1u )->GetShaderResourceView() );
        m_stencilCube.Draw( graphics.GetContext(), m_cbMatrices, camera );

        // Draw light object
        graphics.UpdateRenderStateTexture();
        m_light.Draw( camera.GetViewMatrix(), camera.GetProjectionMatrix() );
    } );
#pragma endregion

#pragma region RTT_THREADING
    // Offload rtt rendering to separate threads
    uint32_t start = 0u, end = 1u;
    uint32_t threadCount = CAMERA_COUNT;
    for ( uint32_t t = 0u; t < threadCount; ++t )
    {
        ThreadManager::CreateThread( [=]() -> void
        {
            ThreadManager::Lock();
            for ( uint32_t i = start; i < end; ++i )
            {
                for ( uint32_t j = 0u; j < RENDER_DEPTH; ++j )
                {
                    RenderCubeStencils( i, j );
                }
            }
            ThreadManager::Unlock();
        } );
        ++start;
        ++end;
    }
    ThreadManager::WaitForAllThreads();

    start = 0u, end = 1u;
    for ( uint32_t t = 0u; t < threadCount; ++t )
    {
        ThreadManager::CreateThread( [=]() -> void
        {
            ThreadManager::Lock();
            for ( uint32_t i = start; i < end; ++i )
            {
                for ( uint32_t j = 0u; j < RENDER_DEPTH; ++j )
                {
                    RenderCubeInvStencils( i, j );
                }
            }
            ThreadManager::Unlock();
        } );
        ++start;
        ++end;
    }
    ThreadManager::WaitForAllThreads();
#pragma endregion

#pragma region MAIN_SCENE
    // Render normal scene
    graphics.BeginFrame();
    
    graphics.UpdateRenderStateSkysphere();
    m_objSkysphere.Draw( m_camera.GetViewMatrix(), m_camera.GetProjectionMatrix() );
    
    // Update constant buffers
    m_light.UpdateCB( m_camera );

    // Draw stencil cube inverse
    graphics.UpdateRenderStateObject();
    for ( uint32_t i = 0u; i < CAMERA_COUNT; i++ )
        m_stencilCubeInv.SetTexture( (Side)i, graphics.GetCubeInvBuffer( (Side)i, RENDER_DEPTH - 1u )->GetShaderResourceView() );
    m_stencilCubeInv.Draw( graphics.GetContext(), m_cbMatrices, m_camera );

    // Draw stencil cube
    for ( uint32_t i = 0u; i < CAMERA_COUNT; i++ )
        m_stencilCube.SetTexture( (Side)i, graphics.GetCubeBuffer( (Side)i, RENDER_DEPTH - 1u )->GetShaderResourceView() );
    m_stencilCube.Draw( graphics.GetContext(), m_cbMatrices, m_camera );

    // Draw light object
    graphics.UpdateRenderStateTexture();
    m_light.Draw( m_camera.GetViewMatrix(), m_camera.GetProjectionMatrix() );
#pragma endregion

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
    ( m_motionBlur.IsActive() || m_fxaa.IsActive() ) ?
        graphics.RenderSceneToTexture( m_motionBlur.GetCB(), m_fxaa.GetCB() ) :
        m_postProcessing.Bind( graphics.GetContext(), graphics.GetRenderTarget() );

    // Render imgui windows
    m_imgui.BeginRender();
    SpawnControlWindow();
    m_imgui.SpawnInstructionWindow();
    m_motionBlur.SpawnControlWindow( m_fxaa.IsActive() );
    m_fxaa.SpawnControlWindow( m_motionBlur.IsActive() );
    m_postProcessing.SpawnControlWindow(
        m_motionBlur.IsActive(), m_fxaa.IsActive() );
    m_light.SpawnControlWindow();
    m_imgui.EndRender();

    // Present frame
    graphics.EndFrame();

    // Store current viewProj for next render pass
    XMStoreFloat4x4( &m_previousViewProjection,
        XMMatrixTranspose( m_camera.GetViewMatrix() ) *
        XMMatrixTranspose( m_camera.GetProjectionMatrix() ) );
#pragma endregion
}

void Application::SpawnControlWindow()
{
    if ( ImGui::Begin( "Depth Stencil", FALSE, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove ) )
    {
        static bool useStaticCamera = m_bUseStaticCamera;
        ImGui::Checkbox( "Use Static Camera?", &useStaticCamera );
        m_bUseStaticCamera = useStaticCamera;

        ImGui::Text( "Render Depth" );
        static int renderDepth = (int)RENDER_DEPTH;
		ImGui::SliderInt( "##Render Depth", &renderDepth, 1, 5 );
        RENDER_DEPTH = (uint32_t)renderDepth;
    }
    ImGui::End();
}