#include "stdafx.h"
#include "Geometry.h"
#include "Camera.h"
#include <imgui/imgui.h>
#include <dxtk/WICTextureLoader.h>

bool Geometry::CreateCylinder( ID3D11Device* pDevice, ID3D11DeviceContext* pContext )
{
	// Create geometry
	SetPosition( XMFLOAT3( 0.0f, 0.0f, 5.0f ) );
	GeometricPrimitive::CreateCylinder( vertices, indices );
	return InitializeMesh( pDevice, pContext, L"Resources\\Textures\\red.jpg" );
}

bool Geometry::CreateCone( ID3D11Device* pDevice, ID3D11DeviceContext* pContext )
{
	// Create geometry
	SetPosition( XMFLOAT3( 0.0f, 0.0f, -5.0f ) );
	GeometricPrimitive::CreateCone( vertices, indices );
	return InitializeMesh( pDevice, pContext, L"Resources\\Textures\\orange.png" );
}

bool Geometry::CreateDodecahedron( ID3D11Device* pDevice, ID3D11DeviceContext* pContext )
{
	// Create geometry
	SetPosition( XMFLOAT3( -5.0f, 0.0f, 0.0f ) );
	GeometricPrimitive::CreateDodecahedron( vertices, indices );
	return InitializeMesh( pDevice, pContext, L"Resources\\Textures\\yellow.png" );
}

bool Geometry::CreateIcosahedron( ID3D11Device* pDevice, ID3D11DeviceContext* pContext )
{
	// Create geometry
	SetPosition( XMFLOAT3( 5.0f, 0.0f, 0.0f ) );
	GeometricPrimitive::CreateIcosahedron( vertices, indices );
	return InitializeMesh( pDevice, pContext, L"Resources\\Textures\\green.png" );
}

bool Geometry::CreateOctahedron( ID3D11Device* pDevice, ID3D11DeviceContext* pContext )
{
	// Create geometry
	SetPosition( XMFLOAT3( 0.0f, -5.0f, 0.0f ) );
	GeometricPrimitive::CreateOctahedron( vertices, indices );
	return InitializeMesh( pDevice, pContext, L"Resources\\Textures\\blue.png" );
}

bool Geometry::CreateTeapot( ID3D11Device* pDevice, ID3D11DeviceContext* pContext )
{
	// Create geometry
	SetPosition( XMFLOAT3( 0.0f, 5.0f, 0.0f ) );
	GeometricPrimitive::CreateTeapot( vertices, indices );
	return InitializeMesh( pDevice, pContext, L"Resources\\Textures\\purple.png" );
}

bool Geometry::InitializeMesh( ID3D11Device* pDevice, ID3D11DeviceContext* pContext, std::wstring texPath )
{
	try
	{
		// Set position to world origin
		DirectX::XMStoreFloat4x4( &m_World, DirectX::XMMatrixIdentity() );

		HRESULT hr = m_vertexBuffer.Initialize( pDevice, &vertices[0], vertices.size() );
		COM_ERROR_IF_FAILED( hr, "Failed to create object vertex buffer!" );

		hr = m_indexBuffer.Initialize( pDevice, &indices[0], indices.size() );
		COM_ERROR_IF_FAILED( hr, "Failed to create object index buffer!" );

		// Load and setup textures
		hr = DirectX::CreateWICTextureFromFile( pDevice, texPath.c_str(), nullptr, m_pTexture.GetAddressOf() );
		COM_ERROR_IF_FAILED( hr, "Failed to create 'diffuse' texture!" );

		// Setup constant buffer
		hr = m_cbMaterial.Initialize( pDevice, pContext );
		COM_ERROR_IF_FAILED( hr, "Failed to create 'Material' constant buffer!" );
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
	static float cummulativeTime = 0;
	cummulativeTime += dt;

	DirectX::XMMATRIX mSpin = DirectX::XMMatrixRotationY( cummulativeTime );
	DirectX::XMMATRIX mTranslate = DirectX::XMMatrixTranslation( m_position.x, m_position.y, m_position.z );
	DirectX::XMMATRIX world = mTranslate;// * mSpin;
	XMStoreFloat4x4( &m_World, world );
}

void Geometry::UpdateCB()
{
	// Setup material data
	MaterialData materialData;
	materialData.Emissive = m_fEmissive;
	materialData.Ambient = m_fAmbient;
	materialData.Diffuse = m_fDiffuse;
	materialData.Specular = m_fSpecular;
	materialData.SpecularPower = m_fSpecularPower;
	materialData.UseTexture = m_bUseTexture;

	// Add to constant buffer
	m_cbMaterial.data.Material = materialData;
	if ( !m_cbMaterial.ApplyChanges() ) return;
}

void Geometry::UpdateBuffers( ConstantBuffer<Matrices>& cb_vs_matrices, Camera& pCamera )
{
	// Get the game object world transform
    XMMATRIX mGO = XMLoadFloat4x4( &m_World );
	cb_vs_matrices.data.mWorld = mGO;

    // Store the view / projection in a constant buffer for the vertex shader to use
	cb_vs_matrices.data.mView = pCamera.GetViewMatrix();
	cb_vs_matrices.data.mProjection = pCamera.GetProjectionMatrix();
	if ( !cb_vs_matrices.ApplyChanges() ) return;
}

void Geometry::Draw( ID3D11DeviceContext* pContext )
{
	UINT offset = 0;
	pContext->IASetVertexBuffers( 0u, 1u, m_vertexBuffer.GetAddressOf(), m_vertexBuffer.StridePtr(), &offset );
	pContext->IASetIndexBuffer( m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0 );
	pContext->PSSetShaderResources( 0u, 1u, m_pTexture.GetAddressOf() );
	pContext->DrawIndexed( m_indexBuffer.IndexCount(), 0u, 0u );
}