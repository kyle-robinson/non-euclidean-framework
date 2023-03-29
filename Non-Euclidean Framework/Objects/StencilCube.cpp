#include "stdafx.h"
#include "StencilCube.h"
#include "Camera.h"

bool StencilCube::Initialize( ID3D11DeviceContext* pContext, ID3D11Device* pDevice )
{
    try
    {
        // Create cube faces
        for ( uint32_t i = 0u; i < 6u; i++ )
        {
            std::shared_ptr<Face> face = std::make_shared<Face>();
		    if ( !face->Initialize( pContext, pDevice ) )
                return false;
		    face->SetInitialScale( 1.0f, 1.0f, 1.0f );
		    m_pFaces.emplace( (Side)i, std::move( face ) );
            m_pTextures.emplace( (Side)i, nullptr );
        }

        // Update face positions
        m_pFaces.at( Side::FRONT )->SetPosition( XMFLOAT3( 0.0f, 0.0f, -1.0f ) );

        m_pFaces.at( Side::BACK )->SetPosition( XMFLOAT3( 0.0f, 0.0f, 1.0f ) );
        m_pFaces.at( Side::BACK )->SetRotation( XMFLOAT3( 0.0f, XM_PI, 0.0f ) );

        m_pFaces.at( Side::LEFT )->SetPosition( XMFLOAT3( 1.0f, 0.0f, 0.0f ) );
        m_pFaces.at( Side::LEFT )->SetRotation( XMFLOAT3( 0.0f, -XM_PIDIV2, 0.0f ) );

        m_pFaces.at( Side::RIGHT )->SetPosition( XMFLOAT3( -1.0f, 0.0f, 0.0f ) );
        m_pFaces.at( Side::RIGHT )->SetRotation( XMFLOAT3( 0.0f, XM_PIDIV2, 0.0f ) );

        m_pFaces.at( Side::TOP )->SetPosition( XMFLOAT3( 0.0f, 1.0f, 0.0f ) );
        m_pFaces.at( Side::TOP )->SetRotation( XMFLOAT3( XM_PIDIV2, 0.0f, 0.0f ) );

        m_pFaces.at( Side::BOTTOM )->SetPosition( XMFLOAT3( 0.0f, -1.0f, 0.0f ) );
        m_pFaces.at( Side::BOTTOM )->SetRotation( XMFLOAT3( -XM_PIDIV2, 0.0f, 0.0f ) );
    }
    catch ( COMException& exception )
    {
        ErrorLogger::Log( exception );
        return false;
    }
    return true;
}

void StencilCube::SetTexture( Side side, ID3D11ShaderResourceView* pTexture ) noexcept
{
    m_pTextures.at( side ) = pTexture;
}

void StencilCube::SetPosition( float x, float y, float z ) noexcept
{
    position = XMFLOAT3( x, y, z );
    for ( uint32_t i = 0u; i < 6u; i++ )
    {
        switch ( (Side)i )
        {
        case Side::FRONT: m_pFaces.at( (Side)i )->SetPosition( x, y, z - m_pFaces.at( (Side)i )->GetScaleFloat3().z ); break;
        case Side::BACK: m_pFaces.at( (Side)i )->SetPosition( x, y, z + m_pFaces.at( (Side)i )->GetScaleFloat3().z ); break;
        case Side::LEFT: m_pFaces.at( (Side)i )->SetPosition( x + m_pFaces.at( (Side)i )->GetScaleFloat3().x, y, z); break;
        case Side::RIGHT: m_pFaces.at( (Side)i )->SetPosition( x - m_pFaces.at( (Side)i )->GetScaleFloat3().x, y, z ); break;
		case Side::TOP: m_pFaces.at( (Side)i )->SetPosition( x, y + m_pFaces.at( (Side)i )->GetScaleFloat3().y, z ); break;
		case Side::BOTTOM: m_pFaces.at( (Side)i )->SetPosition( x, y - m_pFaces.at( (Side)i )->GetScaleFloat3().y, z ); break;
        }
    }
}

void StencilCube::SetScale( float x, float y, float z ) noexcept
{
    for ( uint32_t i = 0u; i < 6u; i++ )
        m_pFaces.at( (Side)i )->SetScale( x, y, z );
}

void StencilCube::Draw( ID3D11DeviceContext* pContext, ConstantBuffer<Matrices>& cb_vs_matrix, Camera& pCamera )
{
    for ( uint32_t i = 0u; i < 6u; i++ )
    {
        pContext->PSSetShaderResources( 0u, 1u, m_pTextures.at( (Side)i ).GetAddressOf() );
        m_pFaces.at( (Side)i )->Draw( cb_vs_matrix, pCamera );
    }
}

void StencilCube::DrawFace( Side side, ConstantBuffer<Matrices>& cb_vs_matrix, Camera& pCamera )
{
    m_pFaces.at( side )->Draw( cb_vs_matrix, pCamera );
}