#include "stdafx.h"
#include "Level1.h"
#include <queue>
#include <random>
#include <numeric>
#include <dxtk/WICTextureLoader.h>

// Function to calculate distance
float Distance( XMFLOAT3 pos1, XMFLOAT3 pos2 )
{
    // Calculating distance
    return sqrt( pow( pos2.x - pos1.x, 2 ) + pow( pos2.y - pos1.y, 2 ) + pow( pos2.z - pos1.z, 2 ) * 1.0f );
}

void Level1::OnCreate()
{
	try
	{
        // Initialize random functions
        for ( int i = 0; i < 9; i++ ) // 9 == no. of cubes in scene
        {
            std::random_device rd; // obtain a random number from hardware
            std::mt19937 rng( rd() ); // seed the generator
            m_randomNums.push_back( std::vector<int>() );
            m_randomNums.at( i ).resize( GeometryType::Count );
            std::iota( std::begin( m_randomNums.at( i ) ), std::end( m_randomNums.at( i ) ), 0 ); // fill the vector
            std::shuffle( std::begin( m_randomNums.at( i ) ), std::end( m_randomNums.at( i ) ), rng );
        }

        // Initialize constant buffers
        HRESULT hr = m_cbMatrices.Initialize( m_gfx->GetDevice(), m_gfx->GetContext() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'Matrices' constant buffer!" );
        hr = m_cbTextureBorder.Initialize( m_gfx->GetDevice(), m_gfx->GetContext() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'Texture Border' constant buffer!" );

        // Create scene elements
        hr = m_stencilCubeInv.Initialize( m_gfx->GetContext(), m_gfx->GetDevice() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'stencil cube inverse' object!" );

        for ( uint32_t i = 0; i < 9; i++ )
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

    // Order cubes based on distance from camera for rendering
    std::multimap<float, StencilCube> stencilCubesMap;
    std::multimap<float, std::vector<int>> randomNums;
    for ( int i = 0; i < m_stencilCubes.size(); i++ )
    {
        float distToCam = Distance( m_camera->GetPositionFloat3(), m_stencilCubes[i].GetPosition() );
        stencilCubesMap.emplace( distToCam, m_stencilCubes[i] );
        randomNums.emplace( distToCam, m_randomNums[i] );
    }
    int it = 0;
    for ( const auto& cube : stencilCubesMap )
    {
        m_stencilCubes[it] = cube.second;
        it++;
    }
    it = 0;
    for ( const auto& num : randomNums )
    {
        m_randomNums[it] = num.second;
        it++;
    }
    // Invert vector to correctly order cubes
    std::reverse( m_stencilCubes.begin(), m_stencilCubes.end() );
    std::reverse( m_randomNums.begin(), m_randomNums.end() );

    // Draw center stencil cube
    int stencilIdx = 0;
    for ( uint32_t i = 0; i < m_stencilCubes.size(); i++ )
    {
        std::queue<int> randomNums;
        for ( uint32_t j = 0; j < m_randomNums.at( i ).size(); j++ )
            randomNums.push( m_randomNums.at( i ).at( j ) );

        for ( uint32_t j = 0u; j < 6u; j++ )
        {
            m_gfx->GetStencilState( (Side)j, Bind::Stencil::Type::MASK )->Bind( context );
            m_gfx->UpdateRenderStateObject( m_cbTextureBorder.GetAddressOf() );
            ID3D11ShaderResourceView* pTexture = nullptr;
            context->PSSetShaderResources( 0u, 1u, &pTexture );
            m_stencilCubes[i].DrawFace( (Side)j, m_cbMatrices, *m_camera );

            m_gfx->GetStencilState( (Side)j, Bind::Stencil::Type::WRITE )->Bind( context );
            m_gfx->UpdateRenderStateObjectInverse();
            int random = randomNums.front();
            randomNums.pop();
            m_geometries[(GeometryType)random].SetPosition( m_stencilCubes[i].GetPosition() );
            m_geometries[(GeometryType)random].UpdateBuffers( context, m_cbMatrices, *m_camera );
            m_geometries[(GeometryType)random].Draw( context );

            // Reset face properties
            m_gfx->GetStencilState( (Side)j, Bind::Stencil::Type::OFF )->Clear( context, m_gfx->GetDepthStencil()->GetDepthStencilView() );
            m_gfx->GetStencilState( (Side)j, Bind::Stencil::Type::MASK )->Clear( context, m_gfx->GetDepthStencil()->GetDepthStencilView() );
            m_gfx->GetStencilState( (Side)j, Bind::Stencil::Type::WRITE )->Clear( context, m_gfx->GetDepthStencil()->GetDepthStencilView() );
            m_gfx->GetStencilState( (Side)j, Bind::Stencil::Type::OFF )->Bind( context );
            stencilIdx++;
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