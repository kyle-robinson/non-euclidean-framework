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
		    face->SetInitialScale( 1.0f, 1.0f, 0.0f );
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

        // Adjust entire cube position
        //for ( uint32_t i = 0u; i < 6u; i++ )
        //    m_pFaces.at( (Side)i )->AdjustPosition( XMFLOAT3( -2.5f, 0.0f, 0.0f ) );
    }
    catch ( COMException& exception )
    {
        ErrorLogger::Log( exception );
        return false;
    }
    return true;
}

void StencilCube::SetTexture( Side side, ID3D11ShaderResourceView* pTexture )
{
    m_pTextures.at( side ) = pTexture;
}

void StencilCube::Draw( ID3D11DeviceContext* pContext, ConstantBuffer<Matrices>& cb_vs_matrix, Camera& pCamera )
{
    for ( uint32_t i = 0u; i < 6u; i++ )
    {
        pContext->PSSetShaderResources( 0u, 1u, m_pTextures.at( (Side)i ).GetAddressOf() );
        m_pFaces.at( (Side)i )->Draw( cb_vs_matrix, pCamera );
    }
}