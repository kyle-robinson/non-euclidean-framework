#include "stdafx.h"
#include "Geometry.h"
#include "Camera.h"
#include <dxtk/WICTextureLoader.h>

bool Geometry::CreateCylinder( ID3D11Device* pDevice, ID3D11DeviceContext* pContext )
{
	// Create geometry
#if _x64
	SetPosition( XMFLOAT3( 0.0f, 0.0f, 5.0f ) );
#endif
	GeometricPrimitive::CreateCylinder( vertices, indices );
	return InitializeMesh( pDevice, pContext, L"Resources\\Textures\\red.jpg" );
}

bool Geometry::CreateCone( ID3D11Device* pDevice, ID3D11DeviceContext* pContext )
{
	// Create geometry
#if _x64
	SetPosition( XMFLOAT3( 0.0f, 0.0f, -5.0f ) );
#endif
	GeometricPrimitive::CreateCone( vertices, indices );
	return InitializeMesh( pDevice, pContext, L"Resources\\Textures\\orange.png" );
}

bool Geometry::CreateDodecahedron( ID3D11Device* pDevice, ID3D11DeviceContext* pContext )
{
	// Create geometry
#if _x64
	SetPosition( XMFLOAT3( -5.0f, 0.0f, 0.0f ) );
#endif
	GeometricPrimitive::CreateDodecahedron( vertices, indices );
	return InitializeMesh( pDevice, pContext, L"Resources\\Textures\\yellow.png" );
}

bool Geometry::CreateIcosahedron( ID3D11Device* pDevice, ID3D11DeviceContext* pContext )
{
	// Create geometry
#if _x64
	SetPosition( XMFLOAT3( 5.0f, 0.0f, 0.0f ) );
#endif
	GeometricPrimitive::CreateIcosahedron( vertices, indices );
	return InitializeMesh( pDevice, pContext, L"Resources\\Textures\\green.png" );
}

bool Geometry::CreateOctahedron( ID3D11Device* pDevice, ID3D11DeviceContext* pContext )
{
	// Create geometry
#if _x64
	SetPosition( XMFLOAT3( 0.0f, -5.0f, 0.0f ) );
#endif
	GeometricPrimitive::CreateOctahedron( vertices, indices );
	return InitializeMesh( pDevice, pContext, L"Resources\\Textures\\blue.png" );
}

bool Geometry::CreateTeapot( ID3D11Device* pDevice, ID3D11DeviceContext* pContext )
{
	// Create geometry
#if _x64
	SetPosition( XMFLOAT3( 0.0f, 5.0f, 0.0f ) );
#endif
	GeometricPrimitive::CreateTeapot( vertices, indices );
	return InitializeMesh( pDevice, pContext, L"Resources\\Textures\\purple.jpg" );
}

bool Geometry::InitializeMesh( ID3D11Device* pDevice, ID3D11DeviceContext* pContext, std::wstring texPath )
{
	try
	{
		// Set position to world origin
		//XMStoreFloat4x4( &m_World, XMMatrixIdentity() );
#if _x64
		worldMatrix = XMMatrixIdentity();
#endif

		HRESULT hr = m_vertexBuffer.Initialize( pDevice, &vertices[0], vertices.size() );
		COM_ERROR_IF_FAILED( hr, "Failed to create object vertex buffer!" );

		hr = m_indexBuffer.Initialize( pDevice, &indices[0], indices.size() );
		COM_ERROR_IF_FAILED( hr, "Failed to create object index buffer!" );

		// Load and setup textures
		hr = CreateWICTextureFromFile( pDevice, texPath.c_str(), nullptr, m_pTexture.GetAddressOf() );
		COM_ERROR_IF_FAILED( hr, "Failed to create 'diffuse' texture!" );

		// Setup constant buffer
		hr = m_cbMaterial.Initialize( pDevice, pContext );
		COM_ERROR_IF_FAILED( hr, "Failed to create 'Material' constant buffer!" );

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

void Geometry::Update( float dt )
{
#if _x64
	AdjustRotation( dt, 0.0f, dt );
#endif
}

void Geometry::UpdateBuffers( ID3D11DeviceContext* pContext, ConstantBuffer<Matrices>& cb_vs_matrices, Camera& pCamera )
{
#if _x64
	cb_vs_matrices.data.mWorld = XMMatrixTranspose( worldMatrix );
#endif
	cb_vs_matrices.data.mView = XMMatrixTranspose( pCamera.GetViewMatrix() );
	cb_vs_matrices.data.mProjection = XMMatrixTranspose( pCamera.GetProjectionMatrix() );
	if ( !cb_vs_matrices.ApplyChanges() ) return;
	pContext->VSSetConstantBuffers( 0u, 1u, cb_vs_matrices.GetAddressOf() );
}

void Geometry::Draw( ID3D11DeviceContext* pContext )
{
	UINT offset = 0;
	pContext->IASetVertexBuffers( 0u, 1u, m_vertexBuffer.GetAddressOf(), m_vertexBuffer.StridePtr(), &offset );
	pContext->IASetIndexBuffer( m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0 );
	pContext->PSSetShaderResources( 0u, 1u, m_pTexture.GetAddressOf() );
	pContext->DrawIndexed( m_indexBuffer.IndexCount(), 0u, 0u );
}