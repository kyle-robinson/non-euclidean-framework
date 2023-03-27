#include "stdafx.h"
#include "Level3.h"
#include <dxtk/WICTextureLoader.h>

void Level3::OnCreate()
{
	try
	{
        // Initialize constant buffers
        HRESULT hr = m_cbMatrices.Initialize( m_gfx->GetDevice(), m_gfx->GetContext() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'Matrices' constant buffer!" );
        hr = m_cbTextureBorder.Initialize( m_gfx->GetDevice(), m_gfx->GetContext() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'Texture Border' constant buffer!" );

        // Create scene elements
        hr = m_light.Initialize( m_gfx->GetDevice(), m_gfx->GetContext(), m_cbMatrices );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'light' object!" );
        hr = m_cube.InitializeMesh( m_gfx->GetDevice(), m_gfx->GetContext() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'cube' object!" );
        hr = m_stencilCubeInv.Initialize( m_gfx->GetContext(), m_gfx->GetDevice() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'stencil cube inverse' object!" );

        // Initialize systems
        hr = m_mapping.Initialize( m_gfx->GetDevice(), m_gfx->GetContext() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'mapping' system!" );
        hr = m_nonEuclidean.Initialize( m_gfx->GetDevice(), m_gfx->GetContext() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'Non-Euclidean' system!" );

        // Initialize textures
        hr = CreateWICTextureFromFile( m_gfx->GetDevice(), L"Resources\\Textures\\light_TEX.jpg", nullptr, m_pTexture.GetAddressOf() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'diffuse' texture!" );
        for ( uint32_t i = 0u; i < 6u; i++ )
            m_stencilCubeInv.SetTexture( (Side)i, m_pTexture.Get() );

        // Initialize models
        if ( !m_nanosuit.Initialize( "Resources\\Models\\Nanosuit\\nanosuit.obj", m_gfx->GetDevice(), m_gfx->GetContext(), m_cbMatrices ) )
            return;
        m_nanosuit.SetInitialPosition( 0.0f, -4.5f, 0.0f );
        m_nanosuit.SetInitialScale( 0.5f, 0.5f, 0.5f );
	}
	catch ( COMException& exception )
	{
		ErrorLogger::Log( exception );
        return;
	}
}

void Level3::OnSwitch()
{
}

void Level3::RenderFrame()
{
	auto context = m_gfx->GetContext();

    TextureBorder_CB tbData;
    tbData.TextureBorder = m_fTextureBorder;
    m_cbTextureBorder.data = tbData;
    if ( !m_cbTextureBorder.ApplyChanges() ) return;

    m_gfx->UpdateRenderStateObject( m_cbTextureBorder.GetAddressOf() );
    m_gfx->GetRasterizerState( Bind::Rasterizer::Type::SKYSPHERE )->Bind( context );
    m_stencilCubeInv.Draw( context, m_cbMatrices, *m_camera );
    m_gfx->GetRasterizerState( Bind::Rasterizer::Type::SOLID )->Bind( context );

    // Update constant buffers
    m_light.UpdateCB( *m_camera );
    m_mapping.UpdateCB();
    m_nonEuclidean.UpdateCB();
    m_cube.UpdateCB();

    // Render objects
    if ( m_bDrawCube )
    {
        m_gfx->UpdateRenderStateCube();
        m_cube.UpdateBuffers( m_cbMatrices, *m_camera );
        context->VSSetConstantBuffers( 0u, 1u, m_cbMatrices.GetAddressOf() );
        context->VSSetConstantBuffers( 1u, 1u, m_nonEuclidean.GetCB() );
        context->PSSetConstantBuffers( 0u, 1u, m_cube.GetCB() );
        context->PSSetConstantBuffers( 1u, 1u, m_light.GetCB() );
        context->PSSetConstantBuffers( 2u, 1u, m_mapping.GetCB() );
        context->PSSetConstantBuffers( 3u, 1u, m_nonEuclidean.GetCB() );
        m_cube.Draw( context );
    }
    if ( m_bDrawNanosuit )
    {
        m_gfx->UpdateRenderStateModel();
        context->VSSetConstantBuffers( 1u, 1u, m_nonEuclidean.GetCB() );
        context->PSSetConstantBuffers( 0u, 1u, m_cube.GetCB() );
        context->PSSetConstantBuffers( 1u, 1u, m_light.GetCB() );
        context->PSSetConstantBuffers( 3u, 1u, m_nonEuclidean.GetCB() );
        m_nanosuit.Draw( m_camera->GetViewMatrix(), m_camera->GetProjectionMatrix() );
    }

    m_gfx->UpdateRenderStateTexture();
    XMMATRIX view = m_camera->GetViewMatrix();
    XMMATRIX projection = m_camera->GetProjectionMatrix();
    m_light.Draw( view, projection );
}

void Level3::Update( const float dt )
{
    if ( m_nonEuclidean.IsActive() && m_bUpdatePos )
    {
        m_nanosuit.SetPosition( 0.0f, 0.0f, 0.0f );
        m_nanosuit.SetScale( 1.0f, 1.0f, 1.0f );
        m_bUpdatePos = false;
    }
    else if ( !m_nonEuclidean.IsActive() && !m_bUpdatePos )
    {
        m_bUpdatePos = true;
        m_nanosuit.ResetPosition();
        m_nanosuit.ResetScale();
    }
    m_cube.Update( dt );
}

void Level3::SpawnWindows()
{
    m_nonEuclidean.SpawnControlWindow();
    m_mapping.SpawnControlWindow();
    m_cube.SpawnControlWindow();
    m_light.SpawnControlWindow();

    if ( ImGui::Begin( "Model", FALSE, ImGuiWindowFlags_AlwaysAutoResize ) )
    {
        ImGui::Checkbox( "Spawn Cube?", &m_bDrawCube );
        if ( m_bDrawCube )
        {
            ImGui::SetNextItemOpen( true, ImGuiCond_Once );
            if ( ImGui::TreeNode( "Cube Data" ) )
            {
                ImGui::Text( "Position" );
                ImGui::SameLine();
                HelpMarker( SLIDER_HINT_TEXT );
                static XMFLOAT3 cubePosition = m_cube.GetPosition();
                if ( ImGui::DragFloat3( "##Cube Position", &cubePosition.x, 0.01f, -4.5f, 4.5f ) )
                    m_cube.SetPosition( cubePosition );

                ImGui::TreePop();
            }
        }

        ImGui::Checkbox( "Spawn Nanosuit?", &m_bDrawNanosuit );
        if ( m_bDrawNanosuit )
        {
            if ( ImGui::TreeNode( "Nanosuit Data" ) )
            {
                ImGui::Text( "Position" );
                ImGui::SameLine();
                HelpMarker( SLIDER_HINT_TEXT );
                XMFLOAT3 nanosuitPosition = m_nanosuit.GetPositionFloat3();
                if ( ImGui::DragFloat3( "##Nanosuit Position", &nanosuitPosition.x, 0.01f, -4.5f, 4.5f ) )
                    m_nanosuit.SetPosition( nanosuitPosition );

                ImGui::Text( "Rotation" );
                ImGui::SameLine();
                HelpMarker( SLIDER_HINT_TEXT );
                XMFLOAT3 nanosuitRotation = m_nanosuit.GetRotationFloat3();
                if ( ImGui::DragFloat3( "##Nanosuit Rotation", &nanosuitRotation.x, 1.0f, -180.0f, 180.0f ) )
                    m_nanosuit.SetRotation( nanosuitRotation );

                ImGui::Text( "Scale" );
                ImGui::SameLine();
                HelpMarker( SLIDER_HINT_TEXT );
                XMFLOAT3 nanosuitScale = m_nanosuit.GetScaleFloat3();
                if ( ImGui::DragFloat3( "##Nanosuit Scale", &nanosuitScale.x, 0.01f, 0.1f, 1.0f ) )
                    m_nanosuit.SetScale( nanosuitScale.x, nanosuitScale.y );

                ImGui::TreePop();
            }
        }
    }
    ImGui::End();
}