#include "stdafx.h"
#include "Application.h"
#include "ThreadManager.h"
#include <imgui/imgui.h>
#include <dxtk/SimpleMath.h>
#include <dxtk/WICTextureLoader.h>

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
            m_fStencilAspect = XMFLOAT2( 1.0f, 1.0f ); // standard ratio
            camera.SetAspectRatio( m_fStencilAspect.x / m_fStencilAspect.y );
            m_stencilCameras.emplace( (Side)i, std::move( camera ) );
        }

        static float cameraOffset = 10.0f;
        m_stencilCameras.at( Side::FRONT ).SetPosition( XMFLOAT3( 0.0f, 0.0f, -cameraOffset ) );
        m_stencilCameras.at( Side::BACK ).SetPosition( XMFLOAT3( 0.0f, 0.0f, cameraOffset ) );
        m_stencilCameras.at( Side::BACK ).SetRotation( XMFLOAT3( 0.0f, -XM_PI, 0.0f ) );
        m_stencilCameras.at( Side::LEFT ).SetPosition( XMFLOAT3( cameraOffset, 0.0f, 0.0f ) );
        m_stencilCameras.at( Side::LEFT ).SetRotation( XMFLOAT3( 0.0f, -XM_PIDIV2, 0.0f ) );
        m_stencilCameras.at( Side::RIGHT ).SetPosition( XMFLOAT3( -cameraOffset, 0.0f, 0.0f ) );
        m_stencilCameras.at( Side::RIGHT ).SetRotation( XMFLOAT3( 0.0f, XM_PIDIV2, 0.0f ) );
        m_stencilCameras.at( Side::TOP ).SetPosition( XMFLOAT3( 0.0f, cameraOffset, 0.0f ) );
        m_stencilCameras.at( Side::TOP ).SetRotation( XMFLOAT3( XM_PIDIV2, 0.0f, 0.0f ) );
        m_stencilCameras.at( Side::BOTTOM ).SetPosition( XMFLOAT3( 0.0f, -cameraOffset, 0.0f ) );
        m_stencilCameras.at( Side::BOTTOM ).SetRotation( XMFLOAT3( -XM_PIDIV2, 0.0f, 0.0f ) );

        // Initialize constant buffers
        HRESULT hr = m_cbMatrices.Initialize( graphics.GetDevice(), graphics.GetContext() );
	    COM_ERROR_IF_FAILED( hr, "Failed to create 'Matrices' constant buffer!" );
        hr = m_cbTextureBorder.Initialize( graphics.GetDevice(), graphics.GetContext() );
	    COM_ERROR_IF_FAILED( hr, "Failed to create 'Texture Border' constant buffer!" );

        // Initialize game objects
	    hr = m_cube.InitializeMesh( graphics.GetDevice(), graphics.GetContext() );
        COM_ERROR_IF_FAILED(hr, "Failed to create 'cube' object!");
        hr = m_cylinder.CreateCylinder( graphics.GetDevice(), graphics.GetContext() );
        COM_ERROR_IF_FAILED(hr, "Failed to create 'cylinder' object!");
        hr = m_cone.CreateCone( graphics.GetDevice(), graphics.GetContext() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'cone' object!" );
        hr = m_dodecahedron.CreateDodecahedron( graphics.GetDevice(), graphics.GetContext() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'dodecahedron' object!" );
        hr = m_icosahedron.CreateIcosahedron( graphics.GetDevice(), graphics.GetContext() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'icosahedron' object!" );
        hr = m_octahedron.CreateOctahedron( graphics.GetDevice(), graphics.GetContext() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'octahedron' object!" );
        hr = m_teapot.CreateTeapot( graphics.GetDevice(), graphics.GetContext() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'teapot' object!" );

        // Create scene elements
        hr = m_light.Initialize( graphics.GetDevice(), graphics.GetContext(), m_cbMatrices );
	    COM_ERROR_IF_FAILED( hr, "Failed to create 'light' object!" );
        hr = m_face.Initialize( graphics.GetContext(), graphics.GetDevice() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'face' object!" );
        hr = m_stencilCube.Initialize( graphics.GetContext(), graphics.GetDevice() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'stencil cube' object!" );
        hr = m_stencilCubeInv.Initialize( graphics.GetContext(), graphics.GetDevice() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'stencil cube inverse' object!" );
        for ( uint32_t i = 0u; i < RENDER_DEPTH; i++ )
        {
            StencilCubeInv cubeInv;
            hr = cubeInv.Initialize( graphics.GetContext(), graphics.GetDevice() );
            m_stencilCubesInvRecursive.push_back( std::move( cubeInv ) );
        }
        COM_ERROR_IF_FAILED( hr, "Failed to create 'stencil cube inverse recursive' object!" );

        // Initialize systems
        m_postProcessing.Initialize( graphics.GetDevice() );
        hr = m_mapping.Initialize( graphics.GetDevice(), graphics.GetContext() );
	    COM_ERROR_IF_FAILED( hr, "Failed to create 'mapping' system!" );
        hr = m_motionBlur.Initialize( graphics.GetDevice(), graphics.GetContext() );
	    COM_ERROR_IF_FAILED( hr, "Failed to create 'motion blur' system!" );
        hr = m_fxaa.Initialize( graphics.GetDevice(), graphics.GetContext() );
	    COM_ERROR_IF_FAILED( hr, "Failed to create 'FXAA' system!" );
        hr = m_nonEuclidean.Initialize( graphics.GetDevice(), graphics.GetContext() );
	    COM_ERROR_IF_FAILED( hr, "Failed to create 'Non-Euclidean' system!" );

        // Initialize models
        if ( !m_objSkysphere.Initialize( "Resources\\Models\\sphere.obj", graphics.GetDevice(), graphics.GetContext(), m_cbMatrices ) )
		    return false;
        m_objSkysphere.SetInitialScale( 50.0f, 50.0f, 50.0f );

        // Initialize Textures
        hr = CreateDDSTextureFromFile( graphics.GetDevice(), L"Resources\\Textures\\bricks_TEX.dds", nullptr, m_pTexture.GetAddressOf() );
		COM_ERROR_IF_FAILED( hr, "Failed to create 'diffuse' texture!" );

        for ( uint32_t i = 0u; i < (uint32_t)Color::Count; i++ )
		{
            std::wstring texPath = L"Resources\\Textures\\";
            switch ( (Color)i )
            {
				case Color::Red: texPath += L"red.jpg"; break;
                case Color::Orange: texPath += L"orange.png"; break;
				case Color::Yellow: texPath += L"yellow.png"; break;
				case Color::Green: texPath += L"green.png"; break;
				case Color::Blue: texPath += L"blue.png"; break;
				case Color::Purple: texPath += L"purple.png"; break;
			}
            Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;
			hr = CreateWICTextureFromFile( graphics.GetDevice(), texPath.c_str(), nullptr, textureView.GetAddressOf() );
			m_pColorTextures.emplace( (Color)i, std::move( textureView ) );
			COM_ERROR_IF_FAILED( hr, "Failed to create 'color' texture!" );
		}

        RENDER_DEPTH = 0u;
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
    // Usefult for finding dx memory leaks
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

    // Update objects
    m_cylinder.Update( dt );
    m_octahedron.Update( dt );
    m_teapot.Update( dt );
    m_dodecahedron.Update( dt );
    m_icosahedron.Update( dt );
    m_cone.Update( dt );

    // Update input
    m_input.Update( dt );

    // Update skysphere position
    m_objSkysphere.SetPosition( m_camera.GetPositionFloat3() );
}

void Application::Render()
{
    // Determine whether to render recursive rooms
    static bool updateDepth = false;
    if ( RENDER_DEPTH == 0u )
    {
        updateDepth = true;
        RENDER_DEPTH = 1u;
    }

    if ( m_bRepeatingSpace )
    {
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
            Camera camera = m_bStaticCamera ? m_stencilCameras.at( (Side)i_inv ) : m_camera;

            graphics.UpdateRenderStateSkysphere();
            m_objSkysphere.Draw( camera.GetViewMatrix(), camera.GetProjectionMatrix() );

            // Render RTT cube
            graphics.UpdateRenderStateObject( m_cbTextureBorder.GetAddressOf() );
            m_stencilCube.Draw( graphics.GetContext(), m_cbMatrices, camera );
            graphics.GetCubeInvBuffer( (Side)i, j )->BindNull( graphics.GetContext() );
        } );
#pragma endregion

#pragma region RTT_CUBE_INV_RECURSIVE
        std::function<void( uint32_t i, uint32_t j, uint32_t k )> RenderCubeInvRecursiveStencils = std::function( [&]( uint32_t i, uint32_t j, uint32_t k ) -> void
        {
            // Adjust side value to account for inverse cube
            uint32_t k_inv = k;
            if ( k % 2u == 0u )
                k_inv = k + 1u;
            else
                k_inv = k - 1u;

            // Render RTT room face
            graphics.BeginFrameCubeInvRecursive( i, j, (Side)k_inv );
            Camera camera = m_bStaticCamera ? m_stencilCameras.at( (Side)k_inv ) : m_camera;

            graphics.UpdateRenderStateSkysphere();
            m_objSkysphere.Draw( camera.GetViewMatrix(), camera.GetProjectionMatrix() );

            // Render RTT cube
            graphics.UpdateRenderStateObject( m_cbTextureBorder.GetAddressOf() );
            i == 0 ?
                m_stencilCubeInv.Draw( graphics.GetContext(), m_cbMatrices, camera ) :
                m_stencilCubesInvRecursive[i - 1u].Draw( graphics.GetContext(), m_cbMatrices, camera );
            m_stencilCube.Draw( graphics.GetContext(), m_cbMatrices, camera );
        } );
#pragma endregion

#pragma region RTT_GENERATION
        if ( !m_bStencilRoom )
        {
            // Generate inverse cube geometry and render targets
            for ( uint32_t i = 0u; i < CAMERA_COUNT; ++i )
                for ( uint32_t j = 0u; j < RENDER_DEPTH; ++j )
                    RenderCubeInvStencils( i, j );

            if ( !updateDepth )
            {
                // Generate recursive render targets inside inverse cube geometry
                for ( uint32_t i = 0u; i < RENDER_DEPTH; ++i ) // current render depth
                    for ( uint32_t j = 0u; j < CAMERA_COUNT; ++j ) // current camera view
                        for ( uint32_t k = 0u; k < 6u; ++k ) // determine cube side
                            RenderCubeInvRecursiveStencils( i, j, k );
            }
        }
#pragma endregion
    }

    // Render normal scene
    graphics.BeginFrame();

    graphics.UpdateRenderStateSkysphere();
    m_objSkysphere.Draw( m_camera.GetViewMatrix(), m_camera.GetProjectionMatrix() );

    m_light.UpdateCB( m_camera );
    graphics.GetContext()->PSSetConstantBuffers( 1u, 1u, m_light.GetCB() );

    if ( m_bRepeatingSpace )
    {
        TextureBorder_CB tbData;
        tbData.TextureBorder = m_fTextureBorder;
        m_cbTextureBorder.data = tbData;
        if ( !m_cbTextureBorder.ApplyChanges() ) return;

#pragma region TEXTURE_BINDING
        if ( !m_bStencilRoom )
        {
            // Draw stencil cube inverse - room
            graphics.UpdateRenderStateObject( m_cbTextureBorder.GetAddressOf() );
            for ( uint32_t i = 0u; i < CAMERA_COUNT; i++ )
            {
                m_stencilCubeInv.SetTexture( (Side)i, graphics.GetCubeInvBuffer( (Side)i, RENDER_DEPTH - 1u )->GetShaderResourceView() );
            }
            if ( updateDepth )
            {
                if ( m_bRTTRoom )
                    m_stencilCubeInv.Draw( graphics.GetContext(), m_cbMatrices, m_camera );
            }
            else
            {
                for ( uint32_t i = 0u; i < RENDER_DEPTH; i++ )
                {
                    for ( uint32_t j = 0u; j < CAMERA_COUNT; j++ )
                        for ( uint32_t k = 0u; k < 6u; k++ )
                            m_stencilCubesInvRecursive[i].SetTexture( (Side)k, graphics.GetCubeInvRecursiveBuffer( i, j, (Side)k )->GetShaderResourceView() );
                    if ( m_bRTTRoom )
                        m_stencilCubesInvRecursive[i].Draw( graphics.GetContext(), m_cbMatrices, m_camera );
                }
            }
        }
#pragma endregion
#pragma region STENCIL_ROOM
        if ( m_bStencilRoom || m_bStencilRTTRoom )
        {
            for ( uint32_t i = 0u; i < CAMERA_COUNT; i++ )
            {
                // Reset face properties
                graphics.GetStencilState( (Side)i, Bind::Stencil::Type::OFF )->Clear( graphics.GetContext(), graphics.GetDepthStencil()->GetDepthStencilView() );
                graphics.GetStencilState( (Side)i, Bind::Stencil::Type::MASK )->Clear( graphics.GetContext(), graphics.GetDepthStencil()->GetDepthStencilView() );
                graphics.GetStencilState( (Side)i, Bind::Stencil::Type::WRITE )->Clear( graphics.GetContext(), graphics.GetDepthStencil()->GetDepthStencilView() );

                m_stencilCube.SetPosition( 0.0f, 0.0f, 0.0f );
                m_face.SetPosition( XMFLOAT3( 0.0f, 0.0f, 0.0f ) );
                m_face.SetRotation( XMFLOAT3( 0.0f, 0.0f, 0.0f ) );
                m_face.SetScale( 1.0f, 1.0f );

				m_pTexture = nullptr;
				graphics.GetContext()->PSSetShaderResources( 0u, 1u, m_pTexture.GetAddressOf() );

                // Create 1st room with stencil mask and the other rooms with RTT
                std::function<void( Side sideToRender, Side sideToOcclude )> CreateStencilRTTRoom = [&]( Side sideToRender, Side sideToOcclude ) -> void
                {
				    graphics.GetStencilState( (Side)i, Bind::Stencil::Type::MASK )->Bind( graphics.GetContext() );
                    graphics.UpdateRenderStateObject( m_cbTextureBorder.GetAddressOf() );

                    // Stencil Mask - stencil view
                    XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
                    XMFLOAT3 rotation = { 0.0f, 0.0f, 0.0f };
                    switch ( sideToRender )
                    {
                    case Side::FRONT:  position.z = 5.0f;  break;
                    case Side::BACK:   position.z = -5.0f; rotation.y = XM_PI;      break;
                    case Side::LEFT:   position.x = -5.0f; rotation.y = -XM_PIDIV2; break;
                    case Side::RIGHT:  position.x = 5.0f;  rotation.y = XM_PIDIV2;  break;
                    case Side::TOP:    position.y = 5.0f;  rotation.x = -XM_PIDIV2; break;
                    case Side::BOTTOM: position.y = -5.0f; rotation.x = XM_PIDIV2;  break;
                    }
                    m_face.SetScale( 5.0f, 5.0f );
                    m_face.SetPosition( position );
                    m_face.SetRotation( rotation );
                    m_face.Draw( m_cbMatrices, m_camera );

                    // Recenter to the middle of the room within the target stencil view
                    switch ( sideToRender )
                    {
                    case Side::FRONT:  position.z += 5.0f; break;
                    case Side::BACK:   position.z -= 5.0f; break;
                    case Side::LEFT:   position.x -= 5.0f; break;
                    case Side::RIGHT:  position.x += 5.0f; break;
                    case Side::TOP:    position.y += 5.0f; break;
                    case Side::BOTTOM: position.y -= 5.0f; break;
                    }

                    // Stencil overwrite - walls inside stencil
                    for ( uint32_t face = 0u; face < 6u; face++ )
                    {
                        if ( sideToOcclude == (Side)face )
                            continue;

                        XMFLOAT3 positionCopy = position;
                        XMFLOAT3 rotationCopy = rotation;

                        // Target side
                        // - looking at the front stencil wall would mean that the target inside wall would be the front wall within the stencil view
                        // - likewise, if I'm looking up at the top stencil, the target view would be the first RTT texture at the backmost face in the stencil view
                        if ( sideToRender == (Side)face )
                        {
                            switch ( sideToRender )
                            {
                            case Side::FRONT:  positionCopy.z += 5.0f; break;
                            case Side::BACK:   positionCopy.z -= 5.0f; break;
                            case Side::LEFT:   positionCopy.x -= 5.0f; break;
                            case Side::RIGHT:  positionCopy.x += 5.0f; break;
                            case Side::TOP:    positionCopy.y += 5.0f; break;
                            case Side::BOTTOM: positionCopy.y -= 5.0f; break;
                            }
                        }
                        // Other sides
                        // - all of the other views inside the stencil I'm looking at
                        // - if I'm looking at the front room stencil, the 'other' stencils inside the view would be the left, right, top, and bottom walls
                        else
                        {
                            if ( sideToRender == Side::FRONT || sideToRender == Side::BACK )
                            {
                                switch ( (Side)face )
                                {
                                case Side::LEFT:   positionCopy.x -= 5.0f; rotationCopy.y = -XM_PIDIV2; break;
                                case Side::RIGHT:  positionCopy.x += 5.0f; rotationCopy.y = XM_PIDIV2;  break;
                                case Side::TOP:    positionCopy.y += 5.0f; rotationCopy.x = -XM_PIDIV2; break;
                                case Side::BOTTOM: positionCopy.y -= 5.0f; rotationCopy.x = XM_PIDIV2;  break;
                                }
                            }
                            else if ( sideToRender == Side::LEFT || sideToRender == Side::RIGHT )
                            {
                                switch ( (Side)face )
                                {
                                case Side::FRONT:  positionCopy.z += 5.0f; rotationCopy.y = 0.0f;       break;
                                case Side::BACK:   positionCopy.z -= 5.0f; rotationCopy.y = XM_PI;      break;
                                case Side::TOP:    positionCopy.y += 5.0f; rotationCopy.x = -XM_PIDIV2; break;
                                case Side::BOTTOM: positionCopy.y -= 5.0f; rotationCopy.x = XM_PIDIV2;  break;
                                }
                            }
                            else if ( sideToRender == Side::TOP || sideToRender == Side::BOTTOM )
                            {
                                rotationCopy.x = 0.0f;
                                switch ( (Side)face )
                                {
                                case Side::FRONT:  positionCopy.z += 5.0f; rotationCopy.y = 0.0f;       break;
                                case Side::BACK:   positionCopy.z -= 5.0f; rotationCopy.y = XM_PI;      break;
                                case Side::LEFT:   positionCopy.x -= 5.0f; rotationCopy.y = -XM_PIDIV2; break;
                                case Side::RIGHT:  positionCopy.x += 5.0f; rotationCopy.y = XM_PIDIV2;  break;
                                }
                            }
                        }

                        graphics.GetStencilState( (Side)face, Bind::Stencil::Type::WRITE )->Bind( graphics.GetContext() );
                        // Flip textures for top and bottom walls
                        Side textureSide = (Side)face;
                        if ( textureSide == Side::TOP )
                            textureSide = Side::BOTTOM;
                        else if ( textureSide == Side::BOTTOM )
                            textureSide = Side::TOP;

                        // Generate textures for all walls inside the current stencil view
                        if ( updateDepth )
                        {
                            graphics.GetContext()->PSSetShaderResources( 0u, 1u, graphics.GetCubeInvBuffer( textureSide, RENDER_DEPTH - 1u )->GetShaderResourceViewPtr() );
                        }
                        else
                        {
                            for ( uint32_t j = 0u; j < RENDER_DEPTH; j++ )
                            {
                                for ( uint32_t k = 0u; k < 6u; k++ )
                                {
                                    if ( face < m_stencilCubesInvRecursive.size() - 1 )
                                    {
                                        m_stencilCubesInvRecursive[face].SetTexture( (Side)k, graphics.GetCubeInvRecursiveBuffer( face, j, (Side)k )->GetShaderResourceView() );
                                    }
                                }
                            }
                            graphics.GetContext()->PSSetShaderResources( 0u, 1u, m_stencilCubesInvRecursive[RENDER_DEPTH - 1u].GetTextures().at( textureSide ).GetAddressOf() );
                        }

                        m_face.SetPosition( positionCopy );
                        m_face.SetRotation( rotationCopy );
                        m_face.Draw( m_cbMatrices, m_camera );
                    }

                    // Colour cube in stencil
                    m_stencilCube.SetPosition( position.x, position.y, position.z );
                    for ( uint32_t j = 0u; j < CAMERA_COUNT; j++ )
                        m_stencilCube.SetTexture( (Side)j, m_pColorTextures[(Color)j].Get() );
                    m_stencilCube.Draw( graphics.GetContext(), m_cbMatrices, m_camera );
                };

                // Create rooms entirely with stencils
                std::function<void()> CreateStencilRoom = [&]() -> void
                {
                    // Stencil Mask - stencil view
                    m_face.SetScale( 5.0f, 5.0f );
                    m_pTexture = nullptr;
                    graphics.GetContext()->PSSetShaderResources( 0u, 1u, m_pTexture.GetAddressOf() );
                    graphics.UpdateRenderStateObject( m_cbTextureBorder.GetAddressOf() );
                    //graphics.GetStencilState( (Side)i, Bind::Stencil::Type::MASK )->Bind( graphics.GetContext() );

                    // First stencil walls
                    XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
                    XMFLOAT3 rotation = { 0.0f, 0.0f, 0.0f };
                    switch ( (Side)i )
                    {
                    case Side::FRONT:  position.z = 5.0f;  break;
                    case Side::BACK:   position.z = -5.0f; rotation.y = XM_PI;      break;
                    case Side::LEFT:   position.x = -5.0f; rotation.y = -XM_PIDIV2; break;
                    case Side::RIGHT:  position.x = 5.0f;  rotation.y = XM_PIDIV2;  break;
                    case Side::TOP:    position.y = 5.0f;  rotation.x = -XM_PIDIV2; break;
                    case Side::BOTTOM: position.y = -5.0f; rotation.x = XM_PIDIV2;  break;
                    }
                    m_face.SetPosition( position );
                    m_face.SetRotation( rotation );
                    m_face.Draw( m_cbMatrices, m_camera );
                    if ( !updateDepth )
                    {
                        // Recursive stencil walls
                        for ( uint32_t j = 0; j < RENDER_DEPTH; j++ )
                        {
                            graphics.GetStencilState( (Side)j, Bind::Stencil::Type::MASK )->Bind( graphics.GetContext() );
                            for ( uint32_t k = 0; k < 6; k++ ) // each wall at current render depth
                            {
                                if ( (Side)i == Side::FRONT && (Side)k == Side::BACK
                                    || (Side)i == Side::BACK && (Side)k == Side::FRONT
                                    || (Side)i == Side::LEFT && (Side)k == Side::RIGHT
                                    || (Side)i == Side::RIGHT && (Side)k == Side::LEFT
                                    || (Side)i == Side::TOP && (Side)k == Side::BOTTOM
                                    || (Side)i == Side::BOTTOM && (Side)k == Side::TOP )
                                    continue;

                                //if ( j == RENDER_DEPTH - 1 && (Side)i == (Side)k )
                                //    continue;

                                //XMFLOAT3 posCopy = position;
                                XMFLOAT3 depthPos = { 0.0f, 0.0f, 0.0f };
                                rotation = { 0.0f, 0.0f, 0.0f };
                                switch ( (Side)i )
                                {
                                case Side::FRONT:  depthPos.z += 10.0f * ( j + 1 ); break;
                                case Side::BACK:   depthPos.z -= 10.0f * ( j + 1 ); break;
                                case Side::LEFT:   depthPos.x -= 10.0f * ( j + 1 ); break;
                                case Side::RIGHT:  depthPos.x += 10.0f * ( j + 1 ); break;
                                case Side::TOP:    depthPos.y += 10.0f * ( j + 1 ); break;
                                case Side::BOTTOM: depthPos.y -= 10.0f * ( j + 1 ); break;
                                }
                                switch ( (Side)k )
                                {
                                case Side::FRONT:  depthPos.z += 5.0f; break;
                                case Side::BACK:   depthPos.z -= 5.0f; rotation.y = XM_PI;      break;
                                case Side::LEFT:   depthPos.x -= 5.0f; rotation.y = -XM_PIDIV2; break;
                                case Side::RIGHT:  depthPos.x += 5.0f; rotation.y = XM_PIDIV2;  break;
                                case Side::TOP:    depthPos.y += 5.0f; rotation.x = -XM_PIDIV2; break;
                                case Side::BOTTOM: depthPos.y -= 5.0f; rotation.x = XM_PIDIV2;  break;
                                }
                                m_face.SetPosition( depthPos );
                                m_face.SetRotation( rotation );
                                m_face.Draw( m_cbMatrices, m_camera );
                            }
                        }
                    }

                    // Colour cubes in stencil
                    for ( uint32_t j = RENDER_DEPTH; j > 0; j-- )
                    {
                        graphics.GetStencilState( (Side)j, Bind::Stencil::Type::WRITE )->Bind( graphics.GetContext() );
                        XMFLOAT3 cubePos = { 0.0f, 0.0f, 0.0f };
                        switch ( (Side)i )
                        {
                        case Side::FRONT:  cubePos.z = 10.0f * j;  break;
                        case Side::BACK:   cubePos.z = -10.0f * j; break;
                        case Side::LEFT:   cubePos.x = -10.0f * j; break;
                        case Side::RIGHT:  cubePos.x = 10.0f * j;  break;
                        case Side::TOP:    cubePos.y = 10.0f * j;  break;
                        case Side::BOTTOM: cubePos.y = -10.0f * j; break;
                        }
                        //position.z = 10.0f * j;
                        m_stencilCube.SetPosition( cubePos.x, cubePos.y, cubePos.z );
                        for ( uint32_t k = 0u; k < 6u; k++ )
                            m_stencilCube.SetTexture( (Side)k, m_pColorTextures[(Color)k].Get() );
                        m_stencilCube.Draw( graphics.GetContext(), m_cbMatrices, m_camera );
                    }

                    //graphics.UpdateRenderStateSkysphere();
                    //m_objSkysphere.Draw( m_camera.GetViewMatrix(), m_camera.GetProjectionMatrix() );
                };

                // Call correct lambda function to generate cubes/rooms
                if ( m_bStencilRoom )
                {
                    CreateStencilRoom();
                }
                else if ( m_bStencilRTTRoom )
                {
                    switch ( (Side)i )
                    {
                    case Side::FRONT:  CreateStencilRTTRoom( Side::FRONT, Side::BACK ); break;
                    case Side::BACK:   CreateStencilRTTRoom( Side::BACK, Side::FRONT ); break;
                    case Side::LEFT:   CreateStencilRTTRoom( Side::LEFT, Side::RIGHT ); break;
                    case Side::RIGHT:  CreateStencilRTTRoom( Side::RIGHT, Side::LEFT ); break;
                    case Side::TOP:    CreateStencilRTTRoom( Side::TOP, Side::BOTTOM ); break;
                    case Side::BOTTOM: CreateStencilRTTRoom( Side::BOTTOM, Side::TOP ); break;
                    }
                }

                // Reset face properties
                graphics.GetStencilState( (Side)i, Bind::Stencil::Type::OFF )->Clear( graphics.GetContext(), graphics.GetDepthStencil()->GetDepthStencilView() );
                graphics.GetStencilState( (Side)i, Bind::Stencil::Type::MASK )->Clear( graphics.GetContext(), graphics.GetDepthStencil()->GetDepthStencilView() );
                graphics.GetStencilState( (Side)i, Bind::Stencil::Type::WRITE )->Clear( graphics.GetContext(), graphics.GetDepthStencil()->GetDepthStencilView() );

                //m_cylinder.SetPosition( XMFLOAT3( 0.0f, 0.0f, 0.0f ) );
                m_stencilCube.SetPosition( 0.0f, 0.0f, 0.0f );
                m_face.SetPosition( XMFLOAT3( 0.0f, 0.0f, 0.0f ) );
                m_face.SetRotation( XMFLOAT3( 0.0f, 0.0f, 0.0f ) );
                m_face.SetScale( 1.0f, 1.0f );
                graphics.GetStencilState( (Side)i, Bind::Stencil::Type::OFF )->Bind( graphics.GetContext() );
            }
        }
#pragma endregion
#pragma region STENCIL_CUBE
        // Draw center stencil cube
        for ( uint32_t i = 0u; i < CAMERA_COUNT; i++ )
        {
            if ( m_bColourCube )
            {
                m_stencilCube.SetTexture( (Side)i, m_pColorTextures[(Color)i].Get() );
            }
            else if ( m_bStencilCube )
            {
                graphics.GetStencilState( (Side)i, Bind::Stencil::Type::MASK )->Bind( graphics.GetContext() );
                graphics.UpdateRenderStateObject( m_cbTextureBorder.GetAddressOf() );

                m_pTexture = nullptr;
                graphics.GetContext()->PSSetShaderResources( 0u, 1u, m_pTexture.GetAddressOf() );

                switch ( (Side)i )
                {
                case Side::FRONT:
                    m_face.SetScale( 1.0f, 1.0f );
                    m_face.SetPosition( XMFLOAT3( 0.0f, 0.0f, -1.0f ) );
                    m_face.SetRotation( XMFLOAT3( 0.0f, 0.0f, 0.0f ) );
                    m_face.Draw( m_cbMatrices, m_camera );
                    graphics.GetStencilState( (Side)i, Bind::Stencil::Type::WRITE )->Bind( graphics.GetContext() );
                    graphics.UpdateRenderStateObjectInverse();
                    m_cylinder.UpdateBuffers( graphics.GetContext(), m_cbMatrices, m_camera );
                    m_cylinder.Draw( graphics.GetContext() );
                    break;

                case Side::BACK:
                    m_face.SetPosition( XMFLOAT3( 0.0f, 0.0f, 1.0f ) );
                    m_face.SetRotation( XMFLOAT3( 0.0f, XM_PI, 0.0f ) );
                    m_face.Draw( m_cbMatrices, m_camera );
                    graphics.GetStencilState( (Side)i, Bind::Stencil::Type::WRITE )->Bind( graphics.GetContext() );
                    graphics.UpdateRenderStateObjectInverse();
                    m_cone.UpdateBuffers( graphics.GetContext(), m_cbMatrices, m_camera );
                    m_cone.Draw( graphics.GetContext() );
                    break;

                case Side::LEFT:
                    m_face.SetPosition( XMFLOAT3( -1.0f, 0.0f, 0.0f ) );
                    m_face.SetRotation( XMFLOAT3( 0.0f, XM_PIDIV2, 0.0f ) );
                    m_face.Draw( m_cbMatrices, m_camera );
                    graphics.GetStencilState( (Side)i, Bind::Stencil::Type::WRITE )->Bind( graphics.GetContext() );
                    graphics.UpdateRenderStateObjectInverse();
                    m_dodecahedron.UpdateBuffers( graphics.GetContext(), m_cbMatrices, m_camera );
                    m_dodecahedron.Draw( graphics.GetContext() );
                    break;

                case Side::RIGHT:
                    m_face.SetPosition( XMFLOAT3( 1.0f, 0.0f, 0.0f ) );
                    m_face.SetRotation( XMFLOAT3( 0.0f, -XM_PIDIV2, 0.0f ) );
                    m_face.Draw( m_cbMatrices, m_camera );
                    graphics.GetStencilState( (Side)i, Bind::Stencil::Type::WRITE )->Bind( graphics.GetContext() );
                    graphics.UpdateRenderStateObjectInverse();
                    m_icosahedron.UpdateBuffers( graphics.GetContext(), m_cbMatrices, m_camera );
                    m_icosahedron.Draw( graphics.GetContext() );
                    break;

                case Side::TOP:
                    m_face.SetPosition( XMFLOAT3( 0.0f, 1.0f, 0.0f ) );
                    m_face.SetRotation( XMFLOAT3( XM_PIDIV2, 0.0f, 0.0f ) );
                    m_face.Draw( m_cbMatrices, m_camera );
                    graphics.GetStencilState( (Side)i, Bind::Stencil::Type::WRITE )->Bind( graphics.GetContext() );
                    graphics.UpdateRenderStateObjectInverse();
                    m_octahedron.UpdateBuffers( graphics.GetContext(), m_cbMatrices, m_camera );
                    m_octahedron.Draw( graphics.GetContext() );
                    break;

                case Side::BOTTOM:
                    m_face.SetPosition( XMFLOAT3( 0.0f, -1.0f, 0.0f ) );
                    m_face.SetRotation( XMFLOAT3( -XM_PIDIV2, 0.0f, 0.0f ) );
                    m_face.Draw( m_cbMatrices, m_camera );
                    graphics.GetStencilState( (Side)i, Bind::Stencil::Type::WRITE )->Bind( graphics.GetContext() );
                    graphics.UpdateRenderStateObjectInverse();
                    m_teapot.UpdateBuffers( graphics.GetContext(), m_cbMatrices, m_camera );
                    m_teapot.Draw( graphics.GetContext() );
                    break;
                }

                // Reset face properties
                graphics.GetStencilState( (Side)i, Bind::Stencil::Type::OFF )->Clear( graphics.GetContext(), graphics.GetDepthStencil()->GetDepthStencilView() );
                graphics.GetStencilState( (Side)i, Bind::Stencil::Type::MASK )->Clear( graphics.GetContext(), graphics.GetDepthStencil()->GetDepthStencilView() );
                graphics.GetStencilState( (Side)i, Bind::Stencil::Type::WRITE )->Clear( graphics.GetContext(), graphics.GetDepthStencil()->GetDepthStencilView() );
                graphics.GetStencilState( (Side)i, Bind::Stencil::Type::OFF )->Bind( graphics.GetContext() );
            }
        }

        if ( m_bColourCube )
            m_stencilCube.Draw( graphics.GetContext(), m_cbMatrices, m_camera );

        if ( updateDepth )
        {
            updateDepth = false;
            RENDER_DEPTH = 0u;
        }
    }
#pragma endregion
#pragma region NC_CUBE
    else
    {
        // Update constant buffers
        m_light.UpdateCB( m_camera );
        m_mapping.UpdateCB();
        m_nonEuclidean.UpdateCB();
        m_cube.UpdateCB();

        // Render objects
        graphics.UpdateRenderStateCube();
        m_cube.UpdateBuffers( m_cbMatrices, m_camera );
        graphics.GetContext()->VSSetConstantBuffers( 0u, 1u, m_cbMatrices.GetAddressOf() );
        graphics.GetContext()->VSSetConstantBuffers( 1u, 1u, m_nonEuclidean.GetCB() );
        graphics.GetContext()->PSSetConstantBuffers( 0u, 1u, m_cube.GetCB() );
        graphics.GetContext()->PSSetConstantBuffers( 1u, 1u, m_light.GetCB() );
        graphics.GetContext()->PSSetConstantBuffers( 2u, 1u, m_mapping.GetCB() );
        graphics.GetContext()->PSSetConstantBuffers( 3u, 1u, m_nonEuclidean.GetCB() );
        m_cube.Draw( graphics.GetContext() );

        graphics.UpdateRenderStateTexture();
        m_light.Draw( m_camera.GetViewMatrix(), m_camera.GetProjectionMatrix() );
    }
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
    ( m_motionBlur.IsActive() || m_fxaa.IsActive() || m_nonEuclidean.IsActive() ) ?
        graphics.RenderSceneToTexture( m_motionBlur.GetCB(), m_fxaa.GetCB() ) :
        m_postProcessing.Bind( graphics.GetContext(), graphics.GetRenderTarget() );

    // Render imgui windows
    if ( m_input.IsCursorEnabled() )
    {
        m_imgui.BeginRender();
        SpawnControlWindows();
        m_imgui.SpawnInstructionWindow();
        m_motionBlur.SpawnControlWindow( m_fxaa.IsActive() );
        m_fxaa.SpawnControlWindow( m_motionBlur.IsActive() );
        m_postProcessing.SpawnControlWindow(
            m_motionBlur.IsActive(),
            m_fxaa.IsActive() );
        if ( !m_bRepeatingSpace )
        {
            m_nonEuclidean.SpawnControlWindow();
            m_mapping.SpawnControlWindow();
            m_cube.SpawnControlWindow();
        }
        m_light.SpawnControlWindow();
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

void Application::SpawnControlWindows()
{
    if ( ImGui::Begin( "Rendering Data", FALSE, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove ) )
    {
        ImGui::Text( "Cube Type: " );
        ImGui::SameLine();
        static int cubeGroup = 0;
        if ( ImGui::RadioButton( "Colour##Cube", &cubeGroup, 0 ) )
        {
            m_bColourCube = true;
            m_bStencilCube = false;
        }
        ImGui::SameLine();
        if ( ImGui::RadioButton( "Stencil##Cube", &cubeGroup, 1 ) )
        {
            m_bColourCube = false;
            m_bStencilCube = true;
        }

        ImGui::Text( "Room Type: " );
        ImGui::SameLine();
        static int roomGroup = 0;
        if ( ImGui::RadioButton( "RTT##Room", &roomGroup, 0 ) )
        {
            m_bRTTRoom = true;
            m_bStencilRoom = false;
            m_bStencilRTTRoom = false;
        }
        ImGui::SameLine();
        if ( ImGui::RadioButton( "Stencil RTT##Room", &roomGroup, 1 ) )
        {
            m_bRTTRoom = false;
            m_bStencilRoom = false;
            m_bStencilRTTRoom = true;
        }
        ImGui::SameLine();
        if ( ImGui::RadioButton( "Stencil##Room", &roomGroup, 2 ) )
        {
            m_bRTTRoom = false;
            m_bStencilRoom = true;
            m_bStencilRTTRoom = false;
        }

        static bool useRepeatingSpace = m_bRepeatingSpace;
        ImGui::Checkbox( "Repeating Space?", &useRepeatingSpace );
        m_bRepeatingSpace = useRepeatingSpace;

        if ( useRepeatingSpace )
        {
            ImGui::Text( "Render Depth" );
            static int renderDepth = (int)RENDER_DEPTH;
		    ImGui::SliderInt( "##Render Depth", &renderDepth, 0, 5 );
            RENDER_DEPTH = (uint32_t)renderDepth;

            ImGui::Text( "Texture Border" );
            static float textureBorder = m_fTextureBorder;
		    ImGui::SliderFloat( "##Texture Border", &textureBorder, 0.00f, 0.10f, "%.2f" );
            m_fTextureBorder = textureBorder;
        }
    }
    ImGui::End();

    if ( m_bRepeatingSpace )
    {
        if ( ImGui::Begin( "Camera Data", FALSE, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove ) )
        {
            static bool useStaticCamera = m_bStaticCamera;
            ImGui::Checkbox( "Static Camera?", &useStaticCamera );
            m_bStaticCamera = useStaticCamera;

            static bool updateCamera = false;
            static float fov = m_fStencilFov;
            static XMFLOAT2 aspectRatio = m_fStencilAspect;

            ImGui::Text( "FOV" );
            if ( ImGui::SliderFloat( "##Texture Border", &fov, 75.0f, 120.0f, "%1.f" ) )
                updateCamera = true;

            ImGui::Text( "Aspect Ratio" );
            ImGui::PushItemWidth( 100.0f );
            if ( ImGui::SliderFloat( "##Aspect Ratio X", &aspectRatio.x, 1.0f, 16.0f, "%1.f" ) )
                updateCamera = true;
            ImGui::SameLine();
            if ( ImGui::SliderFloat( "##Aspect Ratio Y", &aspectRatio.y, 1.0f, 16.0f, "%1.f" ) )
                updateCamera = true;
            ImGui::PopItemWidth();

            if ( updateCamera )
            {
                m_fStencilFov = fov;
                m_fStencilAspect = aspectRatio;
                for ( uint32_t i = 0u; i < CAMERA_COUNT; i++ )
                    m_stencilCameras.at( (Side)i ).SetProjectionValues( m_fStencilFov,
                        m_fStencilAspect.x / m_fStencilAspect.y, 0.01f, 100.0f );
                updateCamera = false;
            }
        }
        ImGui::End();
    }
}