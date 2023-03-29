#include "stdafx.h"
#include "Level4.h"
#include <DDSTextureLoader.h>
#include <dxtk/WICTextureLoader.h>

void Level4::OnCreate()
{
	try
	{
        // Initialize constant buffers
        HRESULT hr = m_cbMatrices.Initialize( m_gfx->GetDevice(), m_gfx->GetContext() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'Matrices' constant buffer!" );
        hr = m_cbTextureBorder.Initialize( m_gfx->GetDevice(), m_gfx->GetContext() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'Texture Border' constant buffer!" );

        // Create scene elements
        for ( uint32_t i = 0u; i < 18u; i++ )
        {
            StencilCube archCube;
            hr = archCube.Initialize( m_gfx->GetContext(), m_gfx->GetDevice() );
            COM_ERROR_IF_FAILED( hr, "Failed to create 'arch stencil cubes' object!" );
            m_archCubes.push_back( std::move( archCube ) );
        }
        hr = m_stencilCube.Initialize( m_gfx->GetContext(), m_gfx->GetDevice() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'stencil cube' object!" );
        hr = m_stencilCubeInv.Initialize( m_gfx->GetContext(), m_gfx->GetDevice() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'stencil cube inverse' object!" );

        // Initialize textures
        hr = CreateWICTextureFromFile( m_gfx->GetDevice(), L"Resources\\Textures\\netherbrick.png", nullptr, m_pWallTexture.GetAddressOf() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'wall' texture!" );
        hr = CreateWICTextureFromFile( m_gfx->GetDevice(), L"Resources\\Textures\\obsidian_long.jpg", nullptr, m_pArchLongTexture.GetAddressOf() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'arch long' texture!" );
        hr = CreateWICTextureFromFile( m_gfx->GetDevice(), L"Resources\\Textures\\obsidian.jpg", nullptr, m_pArchTexture.GetAddressOf() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'arch' texture!" );
        hr = CreateWICTextureFromFile( m_gfx->GetDevice(), L"Resources\\Textures\\light_TEX.jpg", nullptr, m_pTexture.GetAddressOf() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'diffuse' texture!" );

        // Setup object properties
        for ( uint32_t i = 0u; i < m_archCubes.size(); i++ )
            m_archCubes.at( i ).SetScale( 0.5f, 0.5f, 0.5f );
        // Top
        m_archCubes.at( 0 ).SetPosition( -1.5f, 3.0f, 0.0f );
        m_archCubes.at( 1 ).SetPosition( -0.5f, 3.0f, 0.0f );
        m_archCubes.at( 2 ).SetPosition( 0.5f, 3.0f, 0.0f );
        m_archCubes.at( 3 ).SetPosition( 1.5f, 3.0f, 0.0f );
        // Bottom
        m_archCubes.at( 4 ).SetPosition( -1.5f, -3.0f, 0.0f );
        m_archCubes.at( 5 ).SetPosition( -0.5f, -3.0f, 0.0f );
        m_archCubes.at( 6 ).SetPosition( 0.5f, -3.0f, 0.0f );
        m_archCubes.at( 7 ).SetPosition( 1.5f, -3.0f, 0.0f );
        // Left
        m_archCubes.at( 8 ).SetPosition( -1.5f, -2.0f, 0.0f );
        m_archCubes.at( 9 ).SetPosition( -1.5f, -1.0f, 0.0f );
        m_archCubes.at( 10 ).SetPosition( -1.5f, 0.0f, 0.0f );
        m_archCubes.at( 11 ).SetPosition( -1.5f, 1.0f, 0.0f );
        m_archCubes.at( 12 ).SetPosition( -1.5f, 2.0f, 0.0f );
        // Right
        m_archCubes.at( 13 ).SetPosition( 1.5f, -2.0f, 0.0f );
        m_archCubes.at( 14 ).SetPosition( 1.5f, -1.0f, 0.0f );
        m_archCubes.at( 15 ).SetPosition( 1.5f, 0.0f, 0.0f );
        m_archCubes.at( 16 ).SetPosition( 1.5f, 1.0f, 0.0f );
        m_archCubes.at( 17 ).SetPosition( 1.5f, 2.0f, 0.0f );
	}
	catch ( COMException& exception )
	{
		ErrorLogger::Log( exception );
        return;
	}
}

void Level4::OnSwitch()
{
}

void Level4::RenderFrame()
{
	auto context = m_gfx->GetContext();

    TextureBorder_CB tbData;
    tbData.TextureBorder = m_fTextureBorder;
    m_cbTextureBorder.data = tbData;
    if ( !m_cbTextureBorder.ApplyChanges() ) return;

    // Reset stencil inverse room properties
    m_stencilCubeInv.GetFaces().at( Side::FRONT )->SetPosition( 0.0f, 0.0f, 5.0f );
    m_stencilCubeInv.GetFaces().at( Side::BACK )->SetPosition( 0.0f, 0.0f, -5.0f );
    m_stencilCubeInv.GetFaces().at( Side::LEFT )->SetScale( 5.0f, 5.0f, 1.0f );
    m_stencilCubeInv.GetFaces().at( Side::RIGHT )->SetScale( 5.0f, 5.0f, 1.0f );
    m_stencilCubeInv.GetFaces().at( Side::TOP )->SetScale( 5.0f, 5.0f, 1.0f );
    m_stencilCubeInv.GetFaces().at( Side::BOTTOM )->SetScale( 5.0f, 5.0f, 1.0f );

    // Draw stencil inverse cube
    m_gfx->UpdateRenderStateObject( m_cbTextureBorder.GetAddressOf() );
    m_gfx->GetRasterizerState( Bind::Rasterizer::Type::SKYSPHERE )->Bind( context );
    for ( uint32_t i = 0u; i < 6u; i++ )
        m_stencilCubeInv.SetTexture( (Side)i, m_pTexture.Get() );
    m_stencilCubeInv.Draw( context, m_cbMatrices, *m_camera );
    m_gfx->GetRasterizerState( Bind::Rasterizer::Type::SOLID )->Bind( context );

    // Draw arch borders
    for ( uint32_t i = 0u; i < m_archCubes.size(); i++ )
    {
        for ( uint32_t j = 0u; j < 6u; j++ )
        {
            m_archCubes.at( i ).SetTexture( (Side)j, m_pArchTexture.Get() );
        }
        m_archCubes.at( i ).Draw( context, m_cbMatrices, *m_camera );
    }

    // Reset cube properties
    m_stencilCube.GetFace( Side::FRONT )->SetScale( 1.0f, 2.5f, 1.0f );
    m_stencilCube.GetFace( Side::FRONT )->SetPosition( 0.0f, 0.0f, -0.5f );
    m_stencilCube.GetFace( Side::BACK )->SetScale( 1.0f, 2.5f, 1.0f );
    m_stencilCube.GetFace( Side::BACK )->SetPosition( 0.0f, 0.0f, 0.5f );
    m_stencilCube.GetFace( Side::LEFT )->SetScale( 0.5f, 2.5f, 1.0f );
    m_stencilCube.GetFace( Side::RIGHT )->SetScale( 0.5f, 2.5f, 1.0f );
    m_stencilCube.GetFace( Side::TOP )->SetScale( 1.0f, 0.5f, 1.0f );
    m_stencilCube.GetFace( Side::TOP )->SetPosition( 0.0f, 2.5f, 0.0f );
    m_stencilCube.GetFace( Side::BOTTOM )->SetScale( 1.0f, 0.5f, 1.0f );
    m_stencilCube.GetFace( Side::BOTTOM )->SetPosition( 0.0f, -2.5f, 0.0f );

    // Draw arch walls
    for ( uint32_t i = 0u; i < 6u; i++ )
    {
        // Setup the textures for each face
        if ( (Side)i == Side::FRONT || (Side)i == Side::BACK )
        {
            m_gfx->GetStencilState( (Side)i, Bind::Stencil::Type::MASK )->Bind( context );
            ID3D11ShaderResourceView* pTexture = nullptr;
            context->PSSetShaderResources( 0u, 1u, &pTexture );
        }
        else if ( (Side)i == Side::RIGHT || (Side)i == Side::LEFT )
        {
            context->PSSetShaderResources( 0u, 1u, m_pArchLongTexture.GetAddressOf() );
        }
        else if ( (Side)i == Side::TOP || (Side)i == Side::BOTTOM )
        {
            context->PSSetShaderResources( 0u, 1u, m_pArchTexture.GetAddressOf() );
        }
        m_stencilCube.DrawFace( (Side)i, m_cbMatrices, *m_camera );

        // Draw the geometry inside the arch
        if ( (Side)i == Side::FRONT || (Side)i == Side::BACK )
        {
            // Draw room
            m_gfx->GetStencilState( (Side)i, Bind::Stencil::Type::WRITE )->Bind( context );
            context->PSSetShaderResources( 0u, 1u, m_pWallTexture.GetAddressOf() );
            m_stencilCubeInv.GetFaces().at( Side::FRONT )->SetPosition( 0.0f, 0.0f, m_fRoomDepth );
            m_stencilCubeInv.GetFaces().at( Side::BACK )->SetPosition( 0.0f, 0.0f, -m_fRoomDepth );
            m_stencilCubeInv.GetFaces().at( Side::LEFT )->SetScale( m_fRoomDepth, 5.0f, 1.0f );
            m_stencilCubeInv.GetFaces().at( Side::RIGHT )->SetScale( m_fRoomDepth, 5.0f, 1.0f );
            m_stencilCubeInv.GetFaces().at( Side::TOP )->SetScale( 5.0f, m_fRoomDepth, 1.0f );
            m_stencilCubeInv.GetFaces().at( Side::BOTTOM )->SetScale( 5.0f, m_fRoomDepth, 1.0f );
            for ( uint32_t j = 0u; j < 6u; j++ )
                m_stencilCubeInv.SetTexture( (Side)j, m_pWallTexture.Get() );
            m_stencilCubeInv.Draw( context, m_cbMatrices, *m_camera );

            // Reset face properties
            m_gfx->GetStencilState( (Side)i, Bind::Stencil::Type::OFF )->Clear( context, m_gfx->GetDepthStencil()->GetDepthStencilView() );
            m_gfx->GetStencilState( (Side)i, Bind::Stencil::Type::MASK )->Clear( context, m_gfx->GetDepthStencil()->GetDepthStencilView() );
            m_gfx->GetStencilState( (Side)i, Bind::Stencil::Type::WRITE )->Clear( context, m_gfx->GetDepthStencil()->GetDepthStencilView() );
            m_gfx->GetStencilState( (Side)i, Bind::Stencil::Type::OFF )->Bind( context );
        }
    }
}

void Level4::Update( const float dt )
{

}

void Level4::SpawnWindows()
{
    if ( ImGui::Begin( "Arch", FALSE, ImGuiWindowFlags_AlwaysAutoResize ) )
    {
         ImGui::Text( "Room Depth" );
        ImGui::SameLine();
        HelpMarker( DRAG_HINT_TEXT );
        ImGui::DragFloat( "##Room Depth", &m_fRoomDepth, 0.1f, 5.0f, 20.0f );
    }
    ImGui::End();
}