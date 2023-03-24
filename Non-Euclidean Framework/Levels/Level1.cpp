#include "stdafx.h"
#include "Level1.h"
#include <queue>
#include <random>
#include <numeric>
#include <dxtk/WICTextureLoader.h>

void Level1::OnCreate()
{
	try
	{
        // Initialize random functions
        std::random_device rd; // obtain a random number from hardware
        std::mt19937 rng( rd() ); // seed the generator
        m_randomNums.resize( GeometryType::Count );
        std::iota( std::begin( m_randomNums ), std::end( m_randomNums ), 0 ); // fill the vector
        std::shuffle( std::begin( m_randomNums ), std::end( m_randomNums ), rng );

        // Initialize constant buffers
        HRESULT hr = m_cbMatrices.Initialize( m_gfx->GetDevice(), m_gfx->GetContext() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'Matrices' constant buffer!" );
        hr = m_cbTextureBorder.Initialize( m_gfx->GetDevice(), m_gfx->GetContext() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'Texture Border' constant buffer!" );

        // Create scene elements
        hr = m_face.Initialize( m_gfx->GetContext(), m_gfx->GetDevice() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'face' object!" );
        hr = m_stencilCubeInv.Initialize( m_gfx->GetContext(), m_gfx->GetDevice() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'stencil cube inverse' object!" );

        for ( uint32_t i = 1; i <= 9; i++ )
        {
            StencilCube* stencilCube = new StencilCube();
            hr = stencilCube->Initialize( m_gfx->GetContext(), m_gfx->GetDevice() );
            COM_ERROR_IF_FAILED( hr, "Failed to create 'stencil cube' object!" );
            m_stencilCubes.push_back( std::move( *stencilCube ) );
        }
        float offset = 2.5f;
        m_stencilCubes[1].SetPosition( -offset, offset, -offset );
        m_stencilCubes[2].SetPosition( offset, offset, -offset );
        m_stencilCubes[3].SetPosition( -offset, offset, offset );
        m_stencilCubes[4].SetPosition( offset, offset, offset );
        m_stencilCubes[5].SetPosition( -offset, -offset, -offset );
        m_stencilCubes[6].SetPosition( offset, -offset, -offset );
        m_stencilCubes[7].SetPosition( -offset, -offset, offset );
        m_stencilCubes[8].SetPosition( offset, -offset, offset );

        for ( uint32_t i = 0u; i < GeometryType::Count; i++ )
        {
            switch ( (GeometryType)i )
            {
            case GeometryType::Cylinder: hr = m_geometries[(GeometryType)i].CreateCylinder( m_gfx->GetDevice(), m_gfx->GetContext() );  break;
            case GeometryType::Cone: hr = m_geometries[(GeometryType)i].CreateCone( m_gfx->GetDevice(), m_gfx->GetContext() );  break;
            case GeometryType::Dodecahedron: hr = m_geometries[(GeometryType)i].CreateDodecahedron( m_gfx->GetDevice(), m_gfx->GetContext() );  break;
            case GeometryType::Icosahedron: hr = m_geometries[(GeometryType)i].CreateIcosahedron( m_gfx->GetDevice(), m_gfx->GetContext() );  break;
            case GeometryType::Octahedron: hr = m_geometries[(GeometryType)i].CreateOctahedron( m_gfx->GetDevice(), m_gfx->GetContext() );  break;
            case GeometryType::Teapot: hr = m_geometries[(GeometryType)i].CreateTeapot( m_gfx->GetDevice(), m_gfx->GetContext() );  break;
            }
			COM_ERROR_IF_FAILED( hr, "Failed to create 'geometry' object!" );
		}

        // Initialize textures
        hr = CreateWICTextureFromFile( m_gfx->GetDevice(), L"Resources\\Textures\\light_TEX.jpg", nullptr, m_pTexture.GetAddressOf() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'diffuse' texture!" );
        for ( uint32_t i = 0u; i < 6u; i++ )
            m_stencilCubeInv.SetTexture( (Side)i, m_pTexture.Get() );
	}
	catch ( COMException& exception )
	{
		ErrorLogger::Log( exception );
        return;
	}
}

void Level1::OnSwitch()
{
}

void Level1::RenderFrame()
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

    // Draw center stencil cube
    for ( uint32_t i = 0; i < m_stencilCubes.size(); i++ )
    {
        std::queue<int> randomNums;
        for ( uint32_t i = 0; i < m_randomNums.size(); i++ )
            randomNums.push( m_randomNums.at( i ) );

        for ( uint32_t j = 0u; j < 6u; j++ )
        {
            m_gfx->GetStencilState( (Side)j, Bind::Stencil::Type::MASK )->Bind( context );
            m_gfx->UpdateRenderStateObject( m_cbTextureBorder.GetAddressOf() );

            ID3D11ShaderResourceView* pTexture = nullptr;
            context->PSSetShaderResources( 0u, 1u, &pTexture );

            XMFLOAT3 position = m_stencilCubes[i].GetPosition();
            switch ( (Side)j )
            {
            case Side::FRONT:
                m_face.SetPosition( position.x, position.y, position.z - 1.0f );
                m_face.SetRotation( XMFLOAT3( 0.0f, 0.0f, 0.0f ) );
                break;

            case Side::BACK:
                m_face.SetPosition( position.x, position.y, position.z + 1.0f );
                m_face.SetRotation( XMFLOAT3( 0.0f, XM_PI, 0.0f ) );
                break;

            case Side::LEFT:
                m_face.SetPosition( position.x - 1.0f, position.y, position.z );
                m_face.SetRotation( XMFLOAT3( 0.0f, XM_PIDIV2, 0.0f ) );
                break;

            case Side::RIGHT:
                m_face.SetPosition( position.x + 1.0f, position.y, position.z );
                m_face.SetRotation( XMFLOAT3( 0.0f, -XM_PIDIV2, 0.0f ) );
                break;

            case Side::TOP:
                m_face.SetPosition( position.x, position.y + 1.0f, position.z );
                m_face.SetRotation( XMFLOAT3( XM_PIDIV2, 0.0f, 0.0f ) );
                break;

            case Side::BOTTOM:
                m_face.SetPosition( position.x, position.y - 1.0f, position.z );
                m_face.SetRotation( XMFLOAT3( -XM_PIDIV2, 0.0f, 0.0f ) );
                break;
            }

            m_face.Draw( m_cbMatrices, *m_camera );
            m_gfx->GetStencilState( (Side)j, Bind::Stencil::Type::WRITE )->Bind( context );
            m_gfx->UpdateRenderStateObjectInverse();
            int random = randomNums.front();
            randomNums.pop();
            m_geometries[(GeometryType)random].UpdateBuffers(context, m_cbMatrices, *m_camera );
            m_geometries[(GeometryType)random].Draw( context );

            // Reset face properties
            m_gfx->GetStencilState( (Side)j, Bind::Stencil::Type::OFF )->Clear( context, m_gfx->GetDepthStencil()->GetDepthStencilView() );
            m_gfx->GetStencilState( (Side)j, Bind::Stencil::Type::MASK )->Clear( context, m_gfx->GetDepthStencil()->GetDepthStencilView() );
            m_gfx->GetStencilState( (Side)j, Bind::Stencil::Type::WRITE )->Clear( context, m_gfx->GetDepthStencil()->GetDepthStencilView() );
            m_gfx->GetStencilState( (Side)j, Bind::Stencil::Type::OFF )->Bind( context );
        }

        if ( !randomNums.empty() )
            randomNums.pop();
    }
}

void Level1::Update( const float dt )
{
    // Update objects
    for ( uint32_t i = 0; i < GeometryType::Count; i++ )
        m_geometries[(GeometryType)i].Update( dt );
}