#include "stdafx.h"
#include "Quad.h"

VertexPP verticesPP[] =
{
    { { -1.0f,  1.0f } },
    { {  1.0f,  1.0f } },
    { { -1.0f, -1.0f } },
    { {  1.0f, -1.0f } },
};

WORD indicesPP[] =
{
    0, 1, 2,
    1, 3, 2
};

bool Quad::Initialize( ID3D11Device* device )
{
    try
    {
        HRESULT hr = m_vertexBuffer.Initialize( device, verticesPP, ARRAYSIZE( verticesPP ) );
        COM_ERROR_IF_FAILED( hr, "Failed to create quad vertex buffer!" );
        hr = m_indexBuffer.Initialize( device, indicesPP, ARRAYSIZE( indicesPP ) );
        COM_ERROR_IF_FAILED( hr, "Failed to create quad index buffer!" );
    }
    catch ( COMException& exception )
    {
        ErrorLogger::Log( exception );
        return false;
    }
    return true;
}

void Quad::SetupBuffers( ID3D11DeviceContext* context ) noexcept
{
    UINT offset = 0u;
    context->IASetVertexBuffers( 0u, 1u, m_vertexBuffer.GetAddressOf(), m_vertexBuffer.StridePtr(), &offset );
    context->IASetIndexBuffer( m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u );
}