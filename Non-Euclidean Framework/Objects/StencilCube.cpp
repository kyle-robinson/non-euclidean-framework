#include "stdafx.h"
#include "StencilCube.h"
#include "Camera.h"

#pragma region FACE
VertexOBJ verticesOBJ[] =
{
    { { -1.0f,  1.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f } },
    { {  1.0f,  1.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, -1.0f, 0.0f } },
    { {  1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, -1.0f, 0.0f } },
    { { -1.0f, -1.0f, 0.0f }, { 0.0f, 1.0f }, { 0.0f, -1.0f, 0.0f } }
};

WORD indicesOBJ[] =
{
    0, 1, 2,
    0, 2, 3
};

bool Face::Initialize( ID3D11DeviceContext* context, ID3D11Device* device )
{
    this->context = context;

    try
    {
        HRESULT hr = vb_plane.Initialize( device, verticesOBJ, ARRAYSIZE( verticesOBJ ) );
        COM_ERROR_IF_FAILED( hr, "Failed to create face vertex buffer!" );
        hr = ib_plane.Initialize( device, indicesOBJ, ARRAYSIZE( indicesOBJ ) );
        COM_ERROR_IF_FAILED( hr, "Failed to create face index buffer!" );

        SetPosition( XMFLOAT3( 0.0f, 0.0f, 0.0f ) );
        SetRotation( XMFLOAT3( 0.0f, 0.0f, 0.0f ) );
        SetScale( 1.0f, 1.0f );
        UpdateMatrix();
    }
    catch ( COMException& exception )
    {
        ErrorLogger::Log( exception );
        return false;
    }

    return true;
}

void Face::Draw( ConstantBuffer<Matrices>& cb_vs_matrix, Camera& pCamera ) noexcept
{
    UINT offset = 0u;
    context->IASetVertexBuffers( 0u, 1u, vb_plane.GetAddressOf(), vb_plane.StridePtr(), &offset );
    context->IASetIndexBuffer( ib_plane.Get(), DXGI_FORMAT_R16_UINT, 0u );

    cb_vs_matrix.data.mWorld = XMMatrixIdentity() * worldMatrix;
    cb_vs_matrix.data.mView = pCamera.GetViewMatrix();
    cb_vs_matrix.data.mProjection = pCamera.GetProjectionMatrix();
    if ( !cb_vs_matrix.ApplyChanges() ) return;

    context->VSSetConstantBuffers( 0u, 1u, cb_vs_matrix.GetAddressOf() );
    context->DrawIndexed( ib_plane.IndexCount(), 0u, 0u );
}
#pragma endregion

#pragma region STENCIL_CUBE
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
#pragma endregion