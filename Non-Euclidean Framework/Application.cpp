#include "stdafx.h"
#include "Application.h"

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

        // Initialize systems
        m_imgui.Initialize( renderWindow.GetHWND(), graphics.GetDevice(), graphics.GetContext() );
        m_postProcessing.Initialize( graphics.GetDevice() );

        // Initialize input 
        m_camera.Initialize( XMFLOAT3( 0.0f, 0.0f, -3.0f ), width, height );
        m_input.Initialize( renderWindow, m_camera );

        // Initialize constant buffers
        HRESULT hr = m_cbMatrices.Initialize( graphics.GetDevice(), graphics.GetContext() );
	    COM_ERROR_IF_FAILED( hr, "Failed to create 'Matrices' constant buffer!" );

        // Initialize game objects
	    hr = m_cube.InitializeMesh( graphics.GetDevice(), graphics.GetContext() );
        COM_ERROR_IF_FAILED(hr, "Failed to create 'cube' object!");

        hr = m_light.Initialize( graphics.GetDevice(), graphics.GetContext() );
	    COM_ERROR_IF_FAILED( hr, "Failed to create 'light' object!" );

        hr = m_mapping.Initialize( graphics.GetDevice(), graphics.GetContext() );
	    COM_ERROR_IF_FAILED( hr, "Failed to create 'mapping' object!" );

        // Initialize models
        if ( !m_skysphere.Initialize( "Resources\\Models\\sphere.obj", graphics.GetDevice(), graphics.GetContext(), m_cbMatrices ) )
		    return false;
	    m_skysphere.SetInitialScale( 50.0f, 50.0f, 50.0f );
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
    m_skysphere.SetPosition( m_camera.GetPositionFloat3() );

    // Update the cube transform, material etc. 
    m_cube.Update( dt, graphics.GetContext() );
}

void Application::Render()
{
    // Setup graphics
    graphics.BeginFrame();

    // Render skyphere first
    m_skysphere.Draw( m_camera.GetViewMatrix(), m_camera.GetProjectionMatrix() );
    graphics.UpdateRenderState();

    // Get the game object world transform
    DirectX::XMMATRIX mGO = XMLoadFloat4x4( m_cube.GetTransform() );
	m_cbMatrices.data.mWorld = DirectX::XMMatrixTranspose( mGO );
    
    // Store the view / projection in a constant buffer for the vertex shader to use
	m_cbMatrices.data.mView = DirectX::XMMatrixTranspose( m_camera.GetViewMatrix() );
	m_cbMatrices.data.mProjection = DirectX::XMMatrixTranspose( m_camera.GetProjectionMatrix() );
	if ( !m_cbMatrices.ApplyChanges() ) return;
    
    // Update constant buffers
    m_light.UpdateCB( m_camera );
    m_mapping.UpdateCB();
    m_cube.UpdateCB();

    // Render objects
    graphics.GetContext()->VSSetConstantBuffers( 0u, 1u, m_cbMatrices.GetAddressOf() );
    graphics.GetContext()->PSSetConstantBuffers( 1u, 1u, m_cube.GetMaterialCB() );
    graphics.GetContext()->PSSetConstantBuffers( 2u, 1u, m_light.GetLightCB() );
    graphics.GetContext()->PSSetConstantBuffers( 3u, 1u, m_mapping.GetMappingCB() );
    m_cube.Draw( graphics.GetContext() );

    // Render scene to texture
    graphics.RenderSceneToTexture();
    m_postProcessing.Bind( graphics.GetContext(), graphics.GetRenderTarget() );

    // Render imgui windows
    m_imgui.BeginRender();
    m_imgui.SpawnInstructionWindow();
    m_postProcessing.SpawnControlWindow();
    m_mapping.SpawnControlWindow();
    m_light.SpawnControlWindow();
    m_cube.SpawnControlWindow();
    m_imgui.EndRender();

    // Present frame
    graphics.EndFrame();
}