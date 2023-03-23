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

        // Initialize systems
        hr = m_mapping.Initialize( m_gfx->GetDevice(), m_gfx->GetContext() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'mapping' system!" );
        hr = m_nonEuclidean.Initialize( m_gfx->GetDevice(), m_gfx->GetContext() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'Non-Euclidean' system!" );

        // Initialize textures
        hr = CreateWICTextureFromFile( m_gfx->GetDevice(), L"Resources\\Textures\\light_TEX.jpg", nullptr, m_pTexture.GetAddressOf() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'diffuse' texture!" );
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

    // Update constant buffers
    m_light.UpdateCB( *m_camera );
    m_mapping.UpdateCB();
    m_nonEuclidean.UpdateCB();
    m_cube.UpdateCB();

    // Render objects
    m_gfx->UpdateRenderStateCube();
    m_cube.UpdateBuffers( m_cbMatrices, *m_camera );
    context->VSSetConstantBuffers( 0u, 1u, m_cbMatrices.GetAddressOf() );
    context->VSSetConstantBuffers( 1u, 1u, m_nonEuclidean.GetCB() );
    context->PSSetConstantBuffers( 0u, 1u, m_cube.GetCB() );
    context->PSSetConstantBuffers( 1u, 1u, m_light.GetCB() );
    context->PSSetConstantBuffers( 2u, 1u, m_mapping.GetCB() );
    context->PSSetConstantBuffers( 3u, 1u, m_nonEuclidean.GetCB() );
    m_cube.Draw( context );

    m_gfx->UpdateRenderStateTexture();
    XMMATRIX view = m_camera->GetViewMatrix();
    XMMATRIX projection = m_camera->GetProjectionMatrix();
    m_light.Draw( view, projection );
}

void Level3::Update( const float dt )
{

}

void Level3::SpawnWindows()
{
    m_nonEuclidean.SpawnControlWindow();
    m_mapping.SpawnControlWindow();
    m_cube.SpawnControlWindow();
    m_light.SpawnControlWindow();
}