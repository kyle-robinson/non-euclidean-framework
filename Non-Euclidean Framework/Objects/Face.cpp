#include "stdafx.h"
#include "Face.h"
#include "Camera.h"

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

#if _x64
        SetPosition( XMFLOAT3( 0.0f, 0.0f, 0.0f ) );
        SetRotation( XMFLOAT3( 0.0f, 0.0f, 0.0f ) );
        SetScale( 1.0f, 1.0f );
        UpdateMatrix();
#endif
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

#if _x64
    cb_vs_matrix.data.mWorld = XMMatrixIdentity() * worldMatrix;
#endif
    cb_vs_matrix.data.mView = pCamera.GetViewMatrix();
    cb_vs_matrix.data.mProjection = pCamera.GetProjectionMatrix();
    if ( !cb_vs_matrix.ApplyChanges() ) return;

    context->VSSetConstantBuffers( 0u, 1u, cb_vs_matrix.GetAddressOf() );
    context->DrawIndexed( ib_plane.IndexCount(), 0u, 0u );
}