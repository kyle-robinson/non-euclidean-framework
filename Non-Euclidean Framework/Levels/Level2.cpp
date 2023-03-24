#include "stdafx.h"
#include "Level2.h"
#include <imgui/imgui.h>
#include <DDSTextureLoader.h>
#include <dxtk/WICTextureLoader.h>

extern uint32_t RENDER_DEPTH;
extern uint32_t CAMERA_COUNT;
extern uint32_t THREAD_COUNT;

void Level2::OnCreate()
{
	try
	{
        // Initialize stencil cameras
        for ( uint32_t i = 0; i < CAMERA_COUNT; i++ )
        {
            Camera camera;
            camera.Initialize( XMFLOAT3( 0.0f, 0.0f, 0.0f ), m_gfx->GetWidth(), m_gfx->GetHeight() );
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
        HRESULT hr = m_cbMatrices.Initialize( m_gfx->GetDevice(), m_gfx->GetContext() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'Matrices' constant buffer!" );
        hr = m_cbTextureBorder.Initialize( m_gfx->GetDevice(), m_gfx->GetContext() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'Texture Border' constant buffer!" );

        // Initialize game objects
        hr = m_stencilCube.Initialize( m_gfx->GetContext(), m_gfx->GetDevice() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'stencil cube' object!" );
        hr = m_stencilCubeInv.Initialize( m_gfx->GetContext(), m_gfx->GetDevice() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'stencil cube inverse' object!" );
        for ( uint32_t i = 0u; i < RENDER_DEPTH; i++ )
        {
            StencilCubeInv cubeInv;
            hr = cubeInv.Initialize( m_gfx->GetContext(), m_gfx->GetDevice() );
            m_stencilCubesInvRecursive.push_back( std::move( cubeInv ) );
        }
        COM_ERROR_IF_FAILED( hr, "Failed to create 'stencil cube inverse recursive' object!" );

        // Create scene elements
        hr = m_face.Initialize( m_gfx->GetContext(), m_gfx->GetDevice() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'face' object!" );
        hr = m_stencilCube.Initialize( m_gfx->GetContext(), m_gfx->GetDevice() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'stencil cube' object!" );

        // Initialize textures
        hr = CreateWICTextureFromFile( m_gfx->GetDevice(), L"Resources\\Textures\\light_TEX.jpg", nullptr, m_pTexture.GetAddressOf() );
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
            case Color::Purple: texPath += L"purple.jpg"; break;
            }
            Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;
            hr = CreateWICTextureFromFile( m_gfx->GetDevice(), texPath.c_str(), nullptr, textureView.GetAddressOf() );
            COM_ERROR_IF_FAILED( hr, "Failed to create 'color' texture!" );
            m_pColorTextures.emplace( (Color)i, std::move( textureView ) );
        }

        for ( uint32_t i = 0u; i < 6u; i++ )
        {
            std::wstring texPath = L"Resources\\Textures\\";
            switch ( (Side)i )
            {
            case Side::FRONT: texPath += L"crate.dds"; break;
            case Side::BACK: texPath += L"darkdirt.dds"; break;
            case Side::LEFT: texPath += L"lightdirt.dds"; break;
            case Side::RIGHT: texPath += L"stone.dds"; break;
            case Side::TOP: texPath += L"grass.dds"; break;
            case Side::BOTTOM: texPath += L"snow.dds"; break;
            }
            Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;
            hr = CreateDDSTextureFromFile( m_gfx->GetDevice(), texPath.c_str(), nullptr, textureView.GetAddressOf() );
            COM_ERROR_IF_FAILED( hr, "Failed to create 'wall' texture!" );
            m_pWallTextures.emplace( (Side)i, std::move( textureView ) );
        }

        RENDER_DEPTH = 0u;

        // Initialize models
        if ( !m_objSkysphere.Initialize( "Resources\\Models\\sphere.obj", m_gfx->GetDevice(), m_gfx->GetContext(), m_cbMatrices ) ) return;
        m_objSkysphere.SetInitialScale( 50.0f, 50.0f, 50.0f );
	}
	catch ( COMException& exception )
	{
		ErrorLogger::Log( exception );
        return;
	}
}

void Level2::OnSwitch()
{
}

void Level2::BeginFrame()
{
    auto context = m_gfx->GetContext();

    // Determine whether to render recursive rooms
    if ( RENDER_DEPTH == 0u )
    {
        m_bUpdateDepth = true;
        RENDER_DEPTH = 1u;
    }

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
            m_gfx->BeginFrameCubeInv( (Side)i_inv, j );
            Camera camera = m_bStaticCamera ? m_stencilCameras.at( (Side)i_inv ) : *m_camera;

            m_gfx->UpdateRenderStateSkysphere();
            m_objSkysphere.Draw( camera.GetViewMatrix(), camera.GetProjectionMatrix() );

            // Render RTT cube
            m_gfx->UpdateRenderStateObject( m_cbTextureBorder.GetAddressOf() );
            m_stencilCube.Draw( context, m_cbMatrices, camera );
            m_gfx->GetCubeInvBuffer( (Side)i, j )->BindNull( context );
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
            m_gfx->BeginFrameCubeInvRecursive( i, j, (Side)k_inv );
            Camera camera = m_bStaticCamera ? m_stencilCameras.at( (Side)k_inv ) : *m_camera;

            m_gfx->UpdateRenderStateSkysphere();
            m_objSkysphere.Draw( camera.GetViewMatrix(), camera.GetProjectionMatrix() );

            // Render RTT cube
            m_gfx->UpdateRenderStateObject( m_cbTextureBorder.GetAddressOf() );
            i == 0 ?
                m_stencilCubeInv.Draw( context, m_cbMatrices, camera ) :
                m_stencilCubesInvRecursive[i - 1u].Draw( context, m_cbMatrices, camera );
            m_stencilCube.Draw( context, m_cbMatrices, camera );
        } );
#pragma endregion

#pragma region RTT_GENERATION
    if ( !m_bStencilRoom )
    {
        // Generate inverse cube geometry and render targets
        for ( uint32_t i = 0u; i < CAMERA_COUNT; ++i )
            for ( uint32_t j = 0u; j < RENDER_DEPTH; ++j )
                RenderCubeInvStencils( i, j );

        if ( !m_bUpdateDepth )
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

void Level2::RenderFrame()
{
	auto context = m_gfx->GetContext();

    TextureBorder_CB tbData;
    tbData.TextureBorder = m_fTextureBorder;
    m_cbTextureBorder.data = tbData;
    if ( !m_cbTextureBorder.ApplyChanges() ) return;

    m_gfx->UpdateRenderStateSkysphere();
    m_objSkysphere.Draw( m_camera->GetViewMatrix(), m_camera->GetProjectionMatrix() );

#pragma region TEXTURE_BINDING
    if ( !m_bStencilRoom )
    {
        // Draw stencil cube inverse - room
        m_gfx->UpdateRenderStateObject( m_cbTextureBorder.GetAddressOf() );
        for ( uint32_t i = 0u; i < CAMERA_COUNT; i++ )
        {
            m_stencilCubeInv.SetTexture( (Side)i, m_gfx->GetCubeInvBuffer( (Side)i, RENDER_DEPTH - 1u )->GetShaderResourceView() );
        }
        if ( m_bUpdateDepth )
        {
            if ( m_bRTTRoom )
                m_stencilCubeInv.Draw( context, m_cbMatrices, *m_camera );
        }
        else
        {
            for ( uint32_t i = 0u; i < RENDER_DEPTH; i++ )
            {
                for ( uint32_t j = 0u; j < CAMERA_COUNT; j++ )
                    for ( uint32_t k = 0u; k < 6u; k++ )
                        m_stencilCubesInvRecursive[i].SetTexture( (Side)k, m_gfx->GetCubeInvRecursiveBuffer( i, j, (Side)k )->GetShaderResourceView() );
                if ( m_bRTTRoom )
                    m_stencilCubesInvRecursive[i].Draw( context, m_cbMatrices, *m_camera );
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
            m_gfx->GetStencilState( (Side)i, Bind::Stencil::Type::OFF )->Clear( context, m_gfx->GetDepthStencil()->GetDepthStencilView() );
            m_gfx->GetStencilState( (Side)i, Bind::Stencil::Type::MASK )->Clear( context, m_gfx->GetDepthStencil()->GetDepthStencilView() );
            m_gfx->GetStencilState( (Side)i, Bind::Stencil::Type::WRITE )->Clear( context, m_gfx->GetDepthStencil()->GetDepthStencilView() );

            m_stencilCube.SetPosition( 0.0f, 0.0f, 0.0f );
            m_face.SetPosition( XMFLOAT3( 0.0f, 0.0f, 0.0f ) );
            m_face.SetRotation( XMFLOAT3( 0.0f, 0.0f, 0.0f ) );
            m_face.SetScale( 1.0f, 1.0f );

            ID3D11ShaderResourceView* pTexture = nullptr;
            context->PSSetShaderResources( 0u, 1u, &pTexture );

            // Create 1st room with stencil mask and the other rooms with RTT
            std::function<void( Side sideToRender, Side sideToOcclude )> CreateStencilRTTRoom = [&]( Side sideToRender, Side sideToOcclude ) -> void
            {
                m_gfx->GetStencilState( (Side)i, Bind::Stencil::Type::MASK )->Bind( context );
                m_gfx->UpdateRenderStateObject( m_cbTextureBorder.GetAddressOf() );

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
                m_face.Draw( m_cbMatrices, *m_camera );

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

                    m_gfx->GetStencilState( (Side)face, Bind::Stencil::Type::WRITE )->Bind( context );
                    // Flip textures for top and bottom walls
                    Side textureSide = (Side)face;
                    if ( textureSide == Side::TOP )
                        textureSide = Side::BOTTOM;
                    else if ( textureSide == Side::BOTTOM )
                        textureSide = Side::TOP;

                    // Generate textures for all walls inside the current stencil view
                    if ( m_bUpdateDepth )
                    {
                        context->PSSetShaderResources( 0u, 1u, m_gfx->GetCubeInvBuffer( textureSide, RENDER_DEPTH - 1u )->GetShaderResourceViewPtr() );
                    }
                    else
                    {
                        for ( uint32_t j = 0u; j < RENDER_DEPTH; j++ )
                        {
                            for ( uint32_t k = 0u; k < 6u; k++ )
                            {
                                if ( face < m_stencilCubesInvRecursive.size() - 1 )
                                {
                                    m_stencilCubesInvRecursive[face].SetTexture( (Side)k, m_gfx->GetCubeInvRecursiveBuffer( face, j, (Side)k )->GetShaderResourceView() );
                                }
                            }
                        }
                        context->PSSetShaderResources( 0u, 1u, m_stencilCubesInvRecursive[RENDER_DEPTH - 1u].GetTextures().at( textureSide ).GetAddressOf() );
                    }

                    m_face.SetPosition( positionCopy );
                    m_face.SetRotation( rotationCopy );
                    m_face.Draw( m_cbMatrices, *m_camera );
                }

                // Colour cube in stencil
                m_stencilCube.SetPosition( position.x, position.y, position.z );
                for ( uint32_t j = 0u; j < CAMERA_COUNT; j++ )
                    m_stencilCube.SetTexture( (Side)j, m_pColorTextures[(Color)j].Get() );
                m_stencilCube.Draw( context, m_cbMatrices, *m_camera );
            };

            // Create rooms entirely with stencils
            std::function<void()> CreateStencilRoom = [&]() -> void
            {
                // Stencil Mask - stencil view
                m_face.SetScale( 5.0f, 5.0f );
                m_gfx->GetStencilState( (Side)i, Bind::Stencil::Type::MASK )->Bind( context );
                m_gfx->UpdateRenderStateObject( m_cbTextureBorder.GetAddressOf() );
                context->PSSetShaderResources( 0u, 1u, m_pTexture.GetAddressOf() );

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
                m_face.Draw( m_cbMatrices, *m_camera );
                if ( m_bUpdateDepth ) return;

                // Recursive stencil walls
                //for ( uint32_t j = RENDER_DEPTH; j > 0; j-- )
                for ( uint32_t j = 0u; j < RENDER_DEPTH; j++ )
                {
                    m_gfx->GetStencilState( (Side)j, Bind::Stencil::Type::MASK )->Bind( context );
                    for ( uint32_t k = 0u; k < 6u; k++ ) // each wall at current render depth
                    {
                        if ( (Side)i == Side::FRONT && (Side)k == Side::BACK
                            || (Side)i == Side::BACK && (Side)k == Side::FRONT
                            || (Side)i == Side::LEFT && (Side)k == Side::RIGHT
                            || (Side)i == Side::RIGHT && (Side)k == Side::LEFT
                            || (Side)i == Side::TOP && (Side)k == Side::BOTTOM
                            || (Side)i == Side::BOTTOM && (Side)k == Side::TOP )
                            continue;

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
                        m_face.Draw( m_cbMatrices, *m_camera );

                        // For each face at that current render depth
                        m_gfx->GetStencilState( (Side)j, Bind::Stencil::Type::WRITE )->Bind( context );
                        for ( uint32_t l = 0u; l < 6u; l++ )
                        {
                            if ( (Side)l == Side::FRONT && (Side)k == Side::BACK
                                || (Side)l == Side::BACK && (Side)k == Side::FRONT
                                || (Side)l == Side::LEFT && (Side)k == Side::RIGHT
                                || (Side)l == Side::RIGHT && (Side)k == Side::LEFT
                                || (Side)l == Side::TOP && (Side)k == Side::BOTTOM
                                || (Side)l == Side::BOTTOM && (Side)k == Side::TOP )
                                continue;

                            XMFLOAT3 depthCopy = depthPos;
                            switch ( (Side)k )
                            {
                            case Side::FRONT:  depthCopy.z += 5.0f; break;
                            case Side::BACK:   depthCopy.z -= 5.0f; break;
                            case Side::LEFT:   depthCopy.x -= 5.0f; break;
                            case Side::RIGHT:  depthCopy.x += 5.0f; break;
                            case Side::TOP:    depthCopy.y += 5.0f; break;
                            case Side::BOTTOM: depthCopy.y -= 5.0f; break;
                            }
                            switch ( (Side)l )
                            {
                            case Side::FRONT:  depthCopy.z += 5.0f; rotation = XMFLOAT3();   break;
                            case Side::BACK:   depthCopy.z -= 5.0f; rotation.y = XM_PI;      break;
                            case Side::LEFT:   depthCopy.x -= 5.0f; rotation.y = -XM_PIDIV2; break;
                            case Side::RIGHT:  depthCopy.x += 5.0f; rotation.y = XM_PIDIV2;  break;
                            case Side::TOP:    depthCopy.y += 5.0f; rotation.x = -XM_PIDIV2; break;
                            case Side::BOTTOM: depthCopy.y -= 5.0f; rotation.x = XM_PIDIV2;  break;
                            }
                            context->PSSetShaderResources( 0u, 1u, m_pWallTextures[(Side)l].GetAddressOf() );
                            m_face.SetPosition( depthCopy );
                            m_face.SetRotation( rotation );
                            m_face.Draw( m_cbMatrices, *m_camera );
                        }
                    }
                }

                // Colour cubes in stencil
                for ( uint32_t j = RENDER_DEPTH; j > 0; j-- )
                {
                    m_gfx->GetStencilState( (Side)j, Bind::Stencil::Type::WRITE )->Bind( context );
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

                    // For each cube at that current render depth
                    for ( uint32_t k = 0; k < 6u; k++ )
                    {
                        XMFLOAT3 cubePosCopy = cubePos;
                        switch ( (Side)k )
                        {
                        case Side::FRONT:  cubePosCopy.z += 10.0f; break;
                        case Side::BACK:   cubePosCopy.z -= 10.0f; break;
                        case Side::LEFT:   cubePosCopy.x -= 10.0f; break;
                        case Side::RIGHT:  cubePosCopy.x += 10.0f; break;
                        case Side::TOP:    cubePosCopy.y += 10.0f; break;
                        case Side::BOTTOM: cubePosCopy.y -= 10.0f; break;
                        }
                        m_stencilCube.SetPosition( cubePosCopy.x, cubePosCopy.y, cubePosCopy.z );
                        for ( uint32_t l = 0u; l < 6u; l++ )
                            m_stencilCube.SetTexture( (Side)l, m_pColorTextures[(Color)l].Get() );
                        m_stencilCube.Draw( context, m_cbMatrices, *m_camera );
                    }

                    m_stencilCube.SetPosition( cubePos.x, cubePos.y, cubePos.z );
                    for ( uint32_t k = 0u; k < 6u; k++ )
                        m_stencilCube.SetTexture( (Side)k, m_pColorTextures[(Color)k].Get() );
                    m_stencilCube.Draw( context, m_cbMatrices, *m_camera );
                }
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
            m_gfx->GetStencilState( (Side)i, Bind::Stencil::Type::OFF )->Clear( context, m_gfx->GetDepthStencil()->GetDepthStencilView() );
            m_gfx->GetStencilState( (Side)i, Bind::Stencil::Type::MASK )->Clear( context, m_gfx->GetDepthStencil()->GetDepthStencilView() );
            m_gfx->GetStencilState( (Side)i, Bind::Stencil::Type::WRITE )->Clear( context, m_gfx->GetDepthStencil()->GetDepthStencilView() );

            m_stencilCube.SetPosition( 0.0f, 0.0f, 0.0f );
            m_face.SetPosition( XMFLOAT3( 0.0f, 0.0f, 0.0f ) );
            m_face.SetRotation( XMFLOAT3( 0.0f, 0.0f, 0.0f ) );
            m_face.SetScale( 1.0f, 1.0f );
            m_gfx->GetStencilState( (Side)i, Bind::Stencil::Type::OFF )->Bind( context );
        }
    }
#pragma endregion
#pragma region COLOR_CUBE
    // Draw center stencil cube
    for ( uint32_t i = 0u; i < CAMERA_COUNT; i++ )
        m_stencilCube.SetTexture( (Side)i, m_pColorTextures[(Color)i].Get() );
    m_stencilCube.Draw( context, m_cbMatrices, *m_camera );

    if ( m_bUpdateDepth )
    {
        m_bUpdateDepth = false;
        RENDER_DEPTH = 0u;
    }
#pragma endregion
}

void Level2::Update( const float dt )
{
    // Update skysphere position
    m_objSkysphere.SetPosition( m_camera->GetPositionFloat3() );
}

void Level2::SpawnWindows()
{
    if ( ImGui::Begin( "Rendering Data", FALSE, ImGuiWindowFlags_AlwaysAutoResize ) )
    {
        static int activeRoomType = 0;
        static bool selectedRoomType[3];
        static std::string previewValueRoomType = "RTT";
        static const char* roomTypeList[]{ "RTT", "Stencil + RTT", "Stencil Masking" };
        ImGui::Text( "Room Type" );
        if ( ImGui::BeginCombo( "##Room Type", previewValueRoomType.c_str() ) )
        {
            for ( uint32_t i = 0; i < IM_ARRAYSIZE( roomTypeList ); i++ )
            {
                const bool isSelected = i == activeRoomType;
                if ( ImGui::Selectable( roomTypeList[i], isSelected ) )
                {
                    activeRoomType = i;
                    previewValueRoomType = roomTypeList[i];
                }
            }

            switch ( activeRoomType )
            {
            case 0: m_bRTTRoom = true; m_bStencilRTTRoom = false; m_bStencilRoom = false; break;
            case 1: m_bRTTRoom = false; m_bStencilRTTRoom = true; m_bStencilRoom = false; break;
            case 2: m_bRTTRoom = false; m_bStencilRTTRoom = false; m_bStencilRoom = true; break;
            }

            ImGui::EndCombo();
        }

        ImGui::Text( "Render Depth" );
        static int renderDepth = (int)RENDER_DEPTH;
        ImGui::SliderInt( "##Render Depth", &renderDepth, 0, 5 );
        RENDER_DEPTH = (uint32_t)renderDepth;

        ImGui::Text( "Texture Border" );
        static float textureBorder = m_fTextureBorder;
        ImGui::SliderFloat( "##Texture Border", &textureBorder, 0.00f, 0.10f, "%.2f" );
        m_fTextureBorder = textureBorder;
    }
    ImGui::End();

    if ( ImGui::Begin( "Stencil Cameras", FALSE, ImGuiWindowFlags_AlwaysAutoResize ) )
    {
        static bool useStaticCamera = m_bStaticCamera;
        ImGui::Checkbox( "Static Camera?", &useStaticCamera );
        m_bStaticCamera = useStaticCamera;

        static bool useCollisions = m_camera->CanCollide();
        ImGui::Checkbox( "World Collisions?", &useCollisions );
        useCollisions ?
            m_camera->EnableCollisions() :
            m_camera->DisableCollisions();

        static bool updateCamera = false;
        static float fov = m_fStencilFov;
        static XMFLOAT2 aspectRatio = m_fStencilAspect;

        ImGui::Text( "FOV" );
        if ( ImGui::SliderFloat( "##Fov", &fov, 75.0f, 120.0f, "%1.f" ) )
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