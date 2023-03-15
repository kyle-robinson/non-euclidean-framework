#include "stdafx.h"
#include "Application.h"
#include "ThreadManager.h"
#include <imgui/imgui.h>
#include <dxtk/SimpleMath.h>
#include <dxtk/WICTextureLoader.h>
//#include <tuple>

extern uint32_t RENDER_DEPTH;
extern uint32_t CAMERA_COUNT;
extern uint32_t THREAD_COUNT;
using namespace DirectX::SimpleMath;

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

        static float cameraOffset = 8.0f;
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

        // Initialize toolkit components
        /*for ( uint32_t i = 0u; i < (uint32_t)Object::Count; i++ )
        {
            std::unique_ptr<BasicEffect> pEffect;
            std::unique_ptr<GeometricPrimitive> pObject;
            Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;

            pEffect = std::make_unique<BasicEffect>( graphics.GetDevice() );
            pEffect->SetTextureEnabled( true );
            pEffect->SetPerPixelLighting( true );
            pEffect->SetLightingEnabled( true );
            pEffect->SetLightEnabled( 0, true );
            pEffect->SetLightDiffuseColor( 0, Colors::White );
            pEffect->SetLightDirection( 0, -Vector3::UnitZ );
            pEffect->SetView( m_camera.GetViewMatrix() );
            pEffect->SetProjection( m_camera.GetProjectionMatrix() );
            pEffect->SetTexture( m_pColorTextures[(Color)i].Get() );

            switch ( (Object)i )
            {
            case Object::Cylinder: { pObject = GeometricPrimitive::CreateCylinder( graphics.GetContext() ); } break;
            case Object::Cone: { pObject = GeometricPrimitive::CreateCone( graphics.GetContext() ); } break;
            case Object::Dodecahedron: { pObject = GeometricPrimitive::CreateDodecahedron( graphics.GetContext() ); } break;
            case Object::Icosahedron: { pObject = GeometricPrimitive::CreateIcosahedron( graphics.GetContext() ); } break;
            case Object::Octahedron: { pObject = GeometricPrimitive::CreateOctahedron( graphics.GetContext() ); } break;
            case Object::Teapot: { pObject = GeometricPrimitive::CreateTeapot( graphics.GetContext() ); } break;
            }
            pObject->CreateInputLayout( pEffect.get(), pInputLayout.ReleaseAndGetAddressOf() );

            m_pObjects.emplace( (Object)i, std::make_tuple( std::move( pEffect ), std::move( pObject ), std::move( pInputLayout ) ) );
        }*/

        RENDER_DEPTH = 1u;
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

    // Update input
    m_input.Update( dt );

    // Update skysphere position
    m_objSkysphere.SetPosition( m_camera.GetPositionFloat3() );
}

void Application::Render()
{
    if ( m_bUseRepeatingSpace )
    {
#pragma region RTT_CUBE
        std::function<void( uint32_t i, uint32_t j )> RenderCubeStencils = std::function( [&]( uint32_t i, uint32_t j ) -> void
        {
            // Render RTT cube
            graphics.BeginFrameCube( (Side)i, j );
            Camera camera = m_bUseStaticCamera ? m_stencilCameras.at( (Side)i ) : m_camera;

            if ( m_bDrawCubeSkysphere )
            {
                graphics.UpdateRenderStateSkysphere();
                m_objSkysphere.Draw( camera.GetViewMatrix(), camera.GetProjectionMatrix() );
            }

            if ( m_bDrawCubeRecursion )
            {
                // Draw face with stencil view - mask pass
                graphics.GetStencilState( (Side)i, Bind::Stencil::Type::MASK )->Bind( graphics.GetContext() );
                graphics.UpdateRenderStateObject( m_cbTextureBorder.GetAddressOf() );
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

                // Draw inner geometry - draw pass
                graphics.GetStencilState( (Side)i, Bind::Stencil::Type::WRITE )->Bind( graphics.GetContext() );
                m_face.SetScale( 0.8f, 0.8f );
                switch ( (Side)i )
                {
                case Side::FRONT: m_face.SetPosition( XMFLOAT3( 0.0f, 0.0f, 2.5f ) ); break;
                case Side::BACK: m_face.SetPosition( XMFLOAT3( 0.0f, 0.0f, -2.5f ) ); break;
                case Side::LEFT: m_face.SetPosition( XMFLOAT3( -2.5f, 0.0f, 0.0f ) ); break;
                case Side::RIGHT: m_face.SetPosition( XMFLOAT3( 2.5f, 0.0f, 0.0f ) ); break;
                case Side::TOP: m_face.SetPosition( XMFLOAT3( 0.0f, -2.5f, 0.0f ) ); break;
                case Side::BOTTOM: m_face.SetPosition( XMFLOAT3( 0.0f, 2.5f, 0.0f ) ); break;
                }
                m_face.Draw( m_cbMatrices, camera );

                // Reset face properties
                graphics.GetStencilState( (Side)i, Bind::Stencil::Type::OFF )->Clear( graphics.GetContext(), graphics.GetDepthStencil()->GetDepthStencilView() );
                graphics.GetStencilState( (Side)i, Bind::Stencil::Type::MASK )->Clear( graphics.GetContext(), graphics.GetDepthStencil()->GetDepthStencilView() );
                graphics.GetStencilState( (Side)i, Bind::Stencil::Type::WRITE )->Clear( graphics.GetContext(), graphics.GetDepthStencil()->GetDepthStencilView() );

                graphics.GetStencilState( (Side)i, Bind::Stencil::Type::OFF )->Bind( graphics.GetContext() );
                m_face.SetPosition( XMFLOAT3( 0.0f, 0.0f, 0.0f ) );
                m_face.SetScale( 1.0f, 1.0f );
            }
            /*else
            {
                // Draw toolkit objects
                switch ( (Side)i )
                {
                case Side::FRONT:
                    std::get<0>( m_pObjects[Object::Cylinder] )->SetView( m_camera.GetViewMatrix() );
                    std::get<0>( m_pObjects[Object::Cylinder] )->SetProjection( m_camera.GetProjectionMatrix() );
                    std::get<1>( m_pObjects[Object::Cylinder] )->Draw( std::get<0>( m_pObjects[Object::Cylinder] ).get(), std::get<2>( m_pObjects[Object::Cylinder] ).Get() );
                    break;
                case Side::BACK:
                    std::get<0>( m_pObjects[Object::Cone] )->SetView( m_camera.GetViewMatrix() );
                    std::get<0>( m_pObjects[Object::Cone] )->SetProjection( m_camera.GetProjectionMatrix() );
                    std::get<1>( m_pObjects[Object::Cone] )->Draw( std::get<0>( m_pObjects[Object::Cone] ).get(), std::get<2>( m_pObjects[Object::Cone] ).Get() );
                    break;
                case Side::LEFT:
                    std::get<0>( m_pObjects[Object::Dodecahedron] )->SetView( m_camera.GetViewMatrix() );
                    std::get<0>( m_pObjects[Object::Dodecahedron] )->SetProjection( m_camera.GetProjectionMatrix() );
                    std::get<1>( m_pObjects[Object::Dodecahedron] )->Draw( std::get<0>( m_pObjects[Object::Dodecahedron] ).get(), std::get<2>( m_pObjects[Object::Dodecahedron] ).Get() );
                    break;
                case Side::RIGHT:
                    std::get<0>( m_pObjects[Object::Icosahedron] )->SetView( m_camera.GetViewMatrix() );
                    std::get<0>( m_pObjects[Object::Icosahedron] )->SetProjection( m_camera.GetProjectionMatrix() );
                    std::get<1>( m_pObjects[Object::Icosahedron] )->Draw( std::get<0>( m_pObjects[Object::Icosahedron] ).get(), std::get<2>( m_pObjects[Object::Icosahedron] ).Get() );
                    break;
                case Side::TOP:
                    std::get<0>( m_pObjects[Object::Octahedron] )->SetView( m_camera.GetViewMatrix() );
                    std::get<0>( m_pObjects[Object::Octahedron] )->SetProjection( m_camera.GetProjectionMatrix() );
                    std::get<1>( m_pObjects[Object::Octahedron] )->Draw( std::get<0>( m_pObjects[Object::Octahedron] ).get(), std::get<2>( m_pObjects[Object::Octahedron] ).Get() );
                    break;
                case Side::BOTTOM:
                    std::get<0>( m_pObjects[Object::Teapot] )->SetView( m_camera.GetViewMatrix() );
                    std::get<0>( m_pObjects[Object::Teapot] )->SetProjection( m_camera.GetProjectionMatrix() );
                    std::get<1>( m_pObjects[Object::Teapot] )->Draw( std::get<0>( m_pObjects[Object::Teapot] ).get(), std::get<2>( m_pObjects[Object::Teapot] ).Get() );
                    break;
                }
            }*/

            graphics.GetCubeBuffer( (Side)i, j )->BindNull( graphics.GetContext() );
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
            //graphics.UpdateRenderStateObject( m_cbTextureBorder.GetAddressOf() );
            //m_stencilCube.Draw( graphics.GetContext(), m_cbMatrices, camera );
            //graphics.GetCubeInvBuffer( (Side)i, j )->BindNull( graphics.GetContext() );
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
            Camera camera = m_bUseStaticCamera ? m_stencilCameras.at( (Side)k_inv ) : m_camera;

            graphics.UpdateRenderStateSkysphere();
            m_objSkysphere.Draw( camera.GetViewMatrix(), camera.GetProjectionMatrix() );

            // Render RTT cube
            graphics.UpdateRenderStateObject( m_cbTextureBorder.GetAddressOf() );
            i == 0 ?
                m_stencilCubeInv.Draw( graphics.GetContext(), m_cbMatrices, camera ) :
                m_stencilCubesInvRecursive[i - 1u].Draw( graphics.GetContext(), m_cbMatrices, camera );
        } );
#pragma endregion

#pragma region RTT_GENERATION
        // Generate cube geometry and render targets
        if ( !m_bDebugCubes )
            for ( uint32_t i = 0u; i < CAMERA_COUNT; ++i )
                for ( uint32_t j = 0u; j < RENDER_DEPTH; ++j )
                    RenderCubeStencils( i, j );

        // Generate inverse cube geometry and render targets
        for ( uint32_t i = 0u; i < CAMERA_COUNT; ++i )
            for ( uint32_t j = 0u; j < RENDER_DEPTH; ++j )
                RenderCubeInvStencils( i, j );

        if ( RENDER_DEPTH > 0u )
        {
            // Generate recursive render targets inside inverse cube geometry
            for ( uint32_t i = 0u; i < RENDER_DEPTH; ++i ) // current render depth
                for ( uint32_t j = 0u; j < CAMERA_COUNT; ++j ) // current camera view
                    for ( uint32_t k = 0u; k < 6u; ++k ) // determine cube side
                        RenderCubeInvRecursiveStencils( i, j, k );
        }
#pragma endregion
    }

#pragma region MAIN_SCENE
    // Render normal scene
    graphics.BeginFrame();

    graphics.UpdateRenderStateSkysphere();
    m_objSkysphere.Draw( m_camera.GetViewMatrix(), m_camera.GetProjectionMatrix() );

    if ( m_bUseRepeatingSpace )
    {
        TextureBorder_CB tbData;
        tbData.TextureBorder = m_fTextureBorder;
        m_cbTextureBorder.data = tbData;
        if ( !m_cbTextureBorder.ApplyChanges() ) return;

        // Determine whether to render recursive rooms
        static bool updateDepth = false;
        if ( RENDER_DEPTH == 0u )
        {
            updateDepth = true;
            RENDER_DEPTH = 1u;
        }

        // Draw stencil cube inverse
        graphics.UpdateRenderStateObject( m_cbTextureBorder.GetAddressOf() );
        for ( uint32_t i = 0u; i < CAMERA_COUNT; i++ )
            m_stencilCubeInv.SetTexture( (Side)i, graphics.GetCubeInvBuffer( (Side)i, RENDER_DEPTH - 1u )->GetShaderResourceView() );
        m_stencilCubeInv.Draw( graphics.GetContext(), m_cbMatrices, m_camera );

        if ( !updateDepth )
        {
            for ( uint32_t i = 0u; i < RENDER_DEPTH; i++ )
            {
                for ( uint32_t j = 0u; j < CAMERA_COUNT; j++ )
                    for ( uint32_t k = 0u; k < 6u; k++ )
                        m_stencilCubesInvRecursive[i].SetTexture( (Side)k, graphics.GetCubeInvRecursiveBuffer( i, j, (Side)k )->GetShaderResourceView() );
                m_stencilCubesInvRecursive[i].Draw( graphics.GetContext(), m_cbMatrices, m_camera );
            }
        }

        // Draw stencil cube
        for ( uint32_t i = 0u; i < CAMERA_COUNT; i++ )
        {
            if ( m_bDebugCubes )
            {
                m_stencilCube.SetTexture( (Side)i, m_pColorTextures[(Color)i].Get() );
                m_stencilCube.Draw( graphics.GetContext(), m_cbMatrices, m_camera );
            }
            else if ( m_bUseStencils )
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
                    m_cylinder.UpdateBuffers( m_cbMatrices, m_camera );
                    m_cylinder.Draw( graphics.GetContext() );
                    break;

                case Side::BACK:
                    m_face.SetPosition( XMFLOAT3( 0.0f, 0.0f, 1.0f ) );
                    m_face.SetRotation( XMFLOAT3( 0.0f, XM_PI, 0.0f ) );
                    m_face.Draw( m_cbMatrices, m_camera );
                    graphics.GetStencilState( (Side)i, Bind::Stencil::Type::WRITE )->Bind( graphics.GetContext() );
                    graphics.UpdateRenderStateObjectInverse();
                    m_cone.UpdateBuffers( m_cbMatrices, m_camera );
                    m_cone.Draw( graphics.GetContext() );
                    break;

                case Side::LEFT:
                    m_face.SetPosition( XMFLOAT3( 1.0f, 0.0f, 0.0f ) );
                    m_face.SetRotation( XMFLOAT3( 0.0f, -XM_PIDIV2, 0.0f ) );
                    m_face.Draw( m_cbMatrices, m_camera );
                    graphics.GetStencilState( (Side)i, Bind::Stencil::Type::WRITE )->Bind( graphics.GetContext() );
                    graphics.UpdateRenderStateObjectInverse();
                    m_dodecahedron.UpdateBuffers( m_cbMatrices, m_camera );
                    m_dodecahedron.Draw( graphics.GetContext() );
                    break;

                case Side::RIGHT:
                    m_face.SetPosition( XMFLOAT3( -1.0f, 0.0f, 0.0f ) );
                    m_face.SetRotation( XMFLOAT3( 0.0f, XM_PIDIV2, 0.0f ) );
                    m_face.Draw( m_cbMatrices, m_camera );
                    graphics.GetStencilState( (Side)i, Bind::Stencil::Type::WRITE )->Bind( graphics.GetContext() );
                    graphics.UpdateRenderStateObjectInverse();
                    m_icosahedron.UpdateBuffers( m_cbMatrices, m_camera );
                    m_icosahedron.Draw( graphics.GetContext() );
                    break;

                case Side::TOP:
                    m_face.SetPosition( XMFLOAT3( 0.0f, 1.0f, 0.0f ) );
                    m_face.SetRotation( XMFLOAT3( XM_PIDIV2, 0.0f, 0.0f ) );
                    m_face.Draw( m_cbMatrices, m_camera );
                    graphics.GetStencilState( (Side)i, Bind::Stencil::Type::WRITE )->Bind( graphics.GetContext() );
                    graphics.UpdateRenderStateObjectInverse();
                    m_octahedron.UpdateBuffers( m_cbMatrices, m_camera );
                    m_octahedron.Draw( graphics.GetContext() );
                    break;

                case Side::BOTTOM:
                    m_face.SetPosition( XMFLOAT3( 0.0f, -1.0f, 0.0f ) );
                    m_face.SetRotation( XMFLOAT3( -XM_PIDIV2, 0.0f, 0.0f ) );
                    m_face.Draw( m_cbMatrices, m_camera );
                    graphics.GetStencilState( (Side)i, Bind::Stencil::Type::WRITE )->Bind( graphics.GetContext() );
                    graphics.UpdateRenderStateObjectInverse();
                    m_teapot.UpdateBuffers( m_cbMatrices, m_camera );
                    m_teapot.Draw( graphics.GetContext() );
                    break;
                }

                // Reset face properties
                graphics.GetStencilState( (Side)i, Bind::Stencil::Type::OFF )->Clear( graphics.GetContext(), graphics.GetDepthStencil()->GetDepthStencilView() );
                graphics.GetStencilState( (Side)i, Bind::Stencil::Type::MASK )->Clear( graphics.GetContext(), graphics.GetDepthStencil()->GetDepthStencilView() );
                graphics.GetStencilState( (Side)i, Bind::Stencil::Type::WRITE )->Clear( graphics.GetContext(), graphics.GetDepthStencil()->GetDepthStencilView() );

                graphics.GetStencilState( (Side)i, Bind::Stencil::Type::OFF )->Bind( graphics.GetContext() );
                m_face.SetPosition( XMFLOAT3( 0.0f, 0.0f, 0.0f ) );
                m_face.SetRotation( XMFLOAT3( 0.0f, 0.0f, 0.0f ) );
                m_face.SetScale( 1.0f, 1.0f );
            }
            else
            {
                m_stencilCube.SetTexture( (Side)i, graphics.GetCubeBuffer( (Side)i, RENDER_DEPTH - 1u )->GetShaderResourceView() );
                m_stencilCube.Draw( graphics.GetContext(), m_cbMatrices, m_camera );
            }
        }

        // Draw face with stencilled cylinder
        /*graphics.GetStencilState( Bind::Stencil::Type::MASK )->Bind( graphics.GetContext() );
        m_face.SetPosition( 0.0f, 0.0f, 0.0f );
        m_face.SetScale( 1.0f, 1.0f );
        m_face.Draw( m_cbMatrices, m_camera );

        graphics.GetStencilState( Bind::Stencil::Type::WRITE )->Bind( graphics.GetContext() );
        m_face.SetScale( 0.8f, 0.8f );
        m_face.SetPosition( 0.0f, 0.0f, 2.5f );
        m_face.Draw( m_cbMatrices, m_camera );

        graphics.GetStencilState( Bind::Stencil::Type::OFF )->Bind( graphics.GetContext() );*/

        if ( updateDepth )
        {
            updateDepth = false;
            RENDER_DEPTH = 0u;
        }
    }
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
        if ( !m_bUseRepeatingSpace )
        {
            m_nonEuclidean.SpawnControlWindow();
            m_mapping.SpawnControlWindow();
            m_light.SpawnControlWindow();
            m_cube.SpawnControlWindow();
        }
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
    if ( ImGui::Begin( "Stencil Cube Data", FALSE, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove ) )
    {
        static bool debugCubes = m_bDebugCubes;
        ImGui::Checkbox( "Debug Cubes?", &debugCubes );
        m_bDebugCubes = debugCubes;

        if ( !debugCubes )
        {
            static bool useStencils = m_bUseStencils;
            ImGui::Checkbox( "Use Stencils?", &useStencils );
            m_bUseStencils = useStencils;

            if ( !useStencils )
            {
                static bool cubeSkysphere = m_bDrawCubeSkysphere;
                ImGui::Checkbox( "Draw Skysphere?", &cubeSkysphere );
                m_bDrawCubeSkysphere = cubeSkysphere;

                static bool useRecursion = m_bDrawCubeRecursion;
                ImGui::Checkbox( "Use Recursion?", &useRecursion );
                m_bDrawCubeRecursion = useRecursion;
            }
        }
    }
    ImGui::End();

    if ( ImGui::Begin( "Rendering Data", FALSE, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove ) )
    {
        static bool useRepeatingSpace = m_bUseRepeatingSpace;
        ImGui::Checkbox( "Repeating Space?", &useRepeatingSpace );
        m_bUseRepeatingSpace = useRepeatingSpace;

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

    if ( m_bUseRepeatingSpace )
    {
        if ( ImGui::Begin( "Camera Data", FALSE, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove ) )
        {
            static bool useStaticCamera = m_bUseStaticCamera;
            ImGui::Checkbox( "Static Camera?", &useStaticCamera );
            m_bUseStaticCamera = useStaticCamera;

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