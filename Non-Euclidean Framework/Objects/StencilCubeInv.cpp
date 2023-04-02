#include "stdafx.h"
#include "StencilCubeInv.h"
#include "Camera.h"

bool StencilCubeInv::Initialize( ID3D11DeviceContext* pContext, ID3D11Device* pDevice )
{
    try
    {
        // Create quad faces
        for ( uint32_t i = 0u; i < 6u; i++ )
        {
            std::shared_ptr<Face> face = std::make_shared<Face>();
		    if ( !face->Initialize( pContext, pDevice ) )
                return false;
#if _x64
		    face->SetInitialScale( 5.0f, 5.0f, 0.0f );
#endif
		    m_pFaces.emplace( (Side)i, std::move( face ) );
            m_pTextures.emplace( (Side)i, nullptr );
        }

#if _x64
        // Update face positions
        m_pFaces.at( Side::FRONT )->SetPosition( XMFLOAT3( 0.0f, 0.0f, 5.0f ) );

        m_pFaces.at( Side::BACK )->SetPosition( XMFLOAT3( 0.0f, 0.0f, -5.0f ) );
        m_pFaces.at( Side::BACK )->SetRotation( XMFLOAT3( 0.0f, XM_PI, 0.0f ) );

        m_pFaces.at( Side::LEFT )->SetPosition( XMFLOAT3( -5.0f, 0.0f, 0.0f ) );
        m_pFaces.at( Side::LEFT )->SetRotation( XMFLOAT3( 0.0f, -XM_PIDIV2, 0.0f ) );

        m_pFaces.at( Side::RIGHT )->SetPosition( XMFLOAT3( 5.0f, 0.0f, 0.0f ) );
        m_pFaces.at( Side::RIGHT )->SetRotation( XMFLOAT3( 0.0f, XM_PIDIV2, 0.0f ) );

        m_pFaces.at( Side::TOP )->SetPosition( XMFLOAT3( 0.0f, -5.0f, 0.0f ) );
        m_pFaces.at( Side::TOP )->SetRotation( XMFLOAT3( XM_PIDIV2, 0.0f, 0.0f ) );

        m_pFaces.at( Side::BOTTOM )->SetPosition( XMFLOAT3( 0.0f, 5.0f, 0.0f ) );
        m_pFaces.at( Side::BOTTOM )->SetRotation( XMFLOAT3( -XM_PIDIV2, 0.0f, 0.0f ) );
#endif
    }
    catch ( COMException& exception )
    {
        ErrorLogger::Log( exception );
        return false;
    }
    return true;
}

void StencilCubeInv::SetTexture( Side side, ID3D11ShaderResourceView* pTexture )
{
    m_pTextures.at( side ) = pTexture;
}

void StencilCubeInv::SetPosition( float x, float y, float z ) noexcept
{
    for ( uint32_t i = 0u; i < 6u; i++ )
    {
        switch ( (Side)i )
        {
#if _x64
        case Side::FRONT: m_pFaces.at( (Side)i )->SetPosition( x, y, z + 5.0f ); break;
        case Side::BACK: m_pFaces.at( (Side)i )->SetPosition( x, y, z - 5.0f ); break;
        case Side::LEFT: m_pFaces.at( (Side)i )->SetPosition( x - 5.0f, y, z ); break;
        case Side::RIGHT: m_pFaces.at( (Side)i )->SetPosition( x + 5.0f, y, z ); break;
        case Side::TOP: m_pFaces.at( (Side)i )->SetPosition( x, y - 5.0f, z ); break;
        case Side::BOTTOM: m_pFaces.at( (Side)i )->SetPosition( x, y + 5.0f, z ); break;
#endif
        }
    }
}

void StencilCubeInv::Draw( ID3D11DeviceContext* pContext, ConstantBuffer<Matrices>& cb_vs_matrix, Camera& pCamera )
{
    for ( uint32_t i = 0u; i < 6u; i++ )
    {
        pContext->PSSetShaderResources( 0u, 1u, m_pTextures.at( (Side)i ).GetAddressOf() );
        m_pFaces.at( (Side)i )->Draw( cb_vs_matrix, pCamera );
    }
}