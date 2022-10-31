#include "stdafx.h"
#include "Cube.h"
#include <imgui/imgui.h>

Vertex vertices[] =
{
	{ { -1.0f,  1.0f, -1.0f }, {  0.0f,  1.0f,  0.0f }, { 1.0f, 0.0f }, { -1.0f, 0.0f,  0.0f }, { 0.0f,  0.0f, -1.0f } },
	{ {  1.0f,  1.0f, -1.0f }, {  0.0f,  1.0f,  0.0f }, { 0.0f, 0.0f }, { -1.0f, 0.0f,  0.0f }, { 0.0f,  0.0f, -1.0f } },
	{ {  1.0f,  1.0f,  1.0f }, {  0.0f,  1.0f,  0.0f }, { 0.0f, 1.0f }, { -1.0f, 0.0f,  0.0f }, { 0.0f,  0.0f, -1.0f } },
	{ { -1.0f,  1.0f,  1.0f }, {  0.0f,  1.0f,  0.0f }, { 1.0f, 1.0f }, { -1.0f, 0.0f,  0.0f }, { 0.0f,  0.0f, -1.0f } },

	{ { -1.0f, -1.0f, -1.0f }, {  0.0f, -1.0f,  0.0f }, { 0.0f, 0.0f }, {  1.0f, 0.0f,  0.0f }, { 0.0f,  0.0f,  1.0f } },
	{ {  1.0f, -1.0f, -1.0f }, {  0.0f, -1.0f,  0.0f }, { 1.0f, 0.0f }, {  1.0f, 0.0f,  0.0f }, { 0.0f,  0.0f,  1.0f } },
	{ {  1.0f, -1.0f,  1.0f }, {  0.0f, -1.0f,  0.0f }, { 1.0f, 1.0f }, {  1.0f, 0.0f,  0.0f }, { 0.0f,  0.0f,  1.0f } },
	{ { -1.0f, -1.0f,  1.0f }, {  0.0f, -1.0f,  0.0f }, { 0.0f, 1.0f }, {  1.0f, 0.0f,  0.0f }, { 0.0f,  0.0f,  1.0f } },

	{ { -1.0f, -1.0f,  1.0f }, { -1.0f,  0.0f,  0.0f }, { 0.0f, 1.0f }, {  0.0f, 0.0f, -1.0f }, { 0.0f, -1.0f,  0.0f } },
	{ { -1.0f, -1.0f, -1.0f }, { -1.0f,  0.0f,  0.0f }, { 1.0f, 1.0f }, {  0.0f, 0.0f, -1.0f }, { 0.0f, -1.0f,  0.0f } },
	{ { -1.0f,  1.0f, -1.0f }, { -1.0f,  0.0f,  0.0f }, { 1.0f, 0.0f }, {  0.0f, 0.0f, -1.0f }, { 0.0f, -1.0f,  0.0f } },
	{ { -1.0f,  1.0f,  1.0f }, { -1.0f,  0.0f,  0.0f }, { 0.0f, 0.0f }, {  0.0f, 0.0f, -1.0f }, { 0.0f, -1.0f,  0.0f } },

	{ {  1.0f, -1.0f,  1.0f }, {  1.0f,  0.0f,  0.0f }, { 1.0f, 1.0f }, {  0.0f, 0.0f,  1.0f }, { 0.0f, -1.0f,  0.0f } },
	{ {  1.0f, -1.0f, -1.0f }, {  1.0f,  0.0f,  0.0f }, { 0.0f, 1.0f }, {  0.0f, 0.0f,  1.0f }, { 0.0f, -1.0f,  0.0f } },
	{ {  1.0f,  1.0f, -1.0f }, {  1.0f,  0.0f,  0.0f }, { 0.0f, 0.0f }, {  0.0f, 0.0f,  1.0f }, { 0.0f, -1.0f,  0.0f } },
	{ {  1.0f,  1.0f,  1.0f }, {  1.0f,  0.0f,  0.0f }, { 1.0f, 0.0f }, {  0.0f, 0.0f,  1.0f }, { 0.0f, -1.0f,  0.0f } },

	{ { -1.0f, -1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f }, { 0.0f, 1.0f }, {  1.0f, 0.0f,  0.0f }, { 0.0f, -1.0f,  0.0f } },
	{ {  1.0f, -1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f }, { 1.0f, 1.0f }, {  1.0f, 0.0f,  0.0f }, { 0.0f, -1.0f,  0.0f } },
	{ {  1.0f,  1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f }, { 1.0f, 0.0f }, {  1.0f, 0.0f,  0.0f }, { 0.0f, -1.0f,  0.0f } },
	{ { -1.0f,  1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f }, { 0.0f, 0.0f }, {  1.0f, 0.0f,  0.0f }, { 0.0f, -1.0f,  0.0f } },

	{ { -1.0f, -1.0f,  1.0f }, {  0.0f,  0.0f,  1.0f }, { 1.0f, 1.0f }, { -1.0f, 0.0f,  0.0f }, { 0.0f, -1.0f,  0.0f } },
	{ {  1.0f, -1.0f,  1.0f }, {  0.0f,  0.0f,  1.0f }, { 0.0f, 1.0f }, { -1.0f, 0.0f,  0.0f }, { 0.0f, -1.0f,  0.0f } },
	{ {  1.0f,  1.0f,  1.0f }, {  0.0f,  0.0f,  1.0f }, { 0.0f, 0.0f }, { -1.0f, 0.0f,  0.0f }, { 0.0f, -1.0f,  0.0f } },
	{ { -1.0f,  1.0f,  1.0f }, {  0.0f,  0.0f,  1.0f }, { 1.0f, 0.0f }, { -1.0f, 0.0f,  0.0f }, { 0.0f, -1.0f,  0.0f } },
};

WORD indices[] =
{
	3,1,0,
	2,1,3,

	6,4,5,
	7,4,6,

	11,9,8,
	10,9,11,

	14,12,13,
	15,12,14,

	19,17,16,
	18,17,19,

	22,20,21,
	23,20,22
};

bool Cube::InitializeMesh( ID3D11Device* pDevice, ID3D11DeviceContext* pContext )
{
	try
	{
		// Set position to world origin
		DirectX::XMStoreFloat4x4( &m_World, DirectX::XMMatrixIdentity() );

		// Create vertex buffer
		HRESULT hr = m_vertexBuffer.Initialize( pDevice, vertices, ARRAYSIZE( vertices ) );
        COM_ERROR_IF_FAILED( hr, "Failed to create cube vertex buffer!" );
		
		// Create index buffer
        hr = m_indexBuffer.Initialize( pDevice, indices, ARRAYSIZE( indices ) );
        COM_ERROR_IF_FAILED( hr, "Failed to create cube index buffer!" );

		// Load and setup textures
		hr = DirectX::CreateDDSTextureFromFile( pDevice, L"Resources\\Textures\\bricks_TEX.dds", nullptr, m_pTextureDiffuse.GetAddressOf() );
		COM_ERROR_IF_FAILED( hr, "Failed to create 'diffuse' texture!" );

		hr = DirectX::CreateDDSTextureFromFile( pDevice, L"Resources\\Textures\\bricks_NORM.dds", nullptr, m_pTextureNormal.GetAddressOf() );
		COM_ERROR_IF_FAILED( hr, "Failed to create 'normal' texture!" );

		hr = DirectX::CreateDDSTextureFromFile( pDevice, L"Resources\\Textures\\bricks_DISP.dds", nullptr, m_pTextureDisplacement.GetAddressOf() );
		COM_ERROR_IF_FAILED( hr, "Failed to create 'displacement' texture!" );

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

void Cube::Update( float dt, ID3D11DeviceContext* pContext )
{
	static float cummulativeTime = 0;
	cummulativeTime += dt;

	DirectX::XMMATRIX mSpin = DirectX::XMMatrixRotationY( cummulativeTime );
	DirectX::XMMATRIX mTranslate = DirectX::XMMatrixTranslation( 0.0f, 0.0f, 0.0f );
	DirectX::XMMATRIX world = mTranslate * mSpin;
	XMStoreFloat4x4( &m_World, world );
}

void Cube::UpdateCB()
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

void Cube::Draw( ID3D11DeviceContext* pContext )
{
	UINT offset = 0;
	pContext->IASetVertexBuffers( 0u, 1u, m_vertexBuffer.GetAddressOf(), m_vertexBuffer.StridePtr(), &offset );
	pContext->IASetIndexBuffer( m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0 );

	pContext->PSSetShaderResources( 0u, 1u, m_pTextureDiffuse.GetAddressOf() );
	pContext->PSSetShaderResources( 1u, 1u, m_pTextureNormal.GetAddressOf() );
	pContext->PSSetShaderResources( 2u, 1u, m_pTextureDisplacement.GetAddressOf() );

	pContext->DrawIndexed( m_indexBuffer.IndexCount(), 0u, 0u );
}

void Cube::SpawnControlWindow()
{
	if ( ImGui::Begin( "Material Data", FALSE, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove ) )
	{
		ImGui::Text( "Emission Color" );
		ImGui::SliderFloat4( "##Emission", &m_fEmissive.x, 0.0f, 1.0f, "%.1f" );
		ImGui::NewLine();

		ImGui::Text( "Ambient Color" );
		ImGui::SliderFloat4( "##Ambient", &m_fAmbient.x, 0.0f, 1.0f, "%.1f" );
		ImGui::NewLine();

		ImGui::Text( "Diffuse Color" );
		ImGui::SliderFloat4( "##Diffuse", &m_fDiffuse.x, 0.0f, 1.0f, "%.1f" );
		ImGui::NewLine();

		ImGui::Text( "Specular Color" );
		ImGui::SliderFloat4( "##Specular", &m_fSpecular.x, 0.0f, 1.0f, "%.1f" );
		ImGui::NewLine();

		ImGui::Text( "Specular Power" );
		ImGui::SliderFloat( "##Spec Power", &m_fSpecularPower, 0.0f, 256.0f, "%1.f" );
		ImGui::NewLine();

		static bool useTexture = m_bUseTexture;
		ImGui::Checkbox( "Use Texture?", &useTexture );
		m_bUseTexture = useTexture;
	}
	ImGui::End();
}