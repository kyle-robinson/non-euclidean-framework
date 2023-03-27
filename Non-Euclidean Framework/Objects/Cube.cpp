#include "stdafx.h"
#include "Cube.h"
#include "Camera.h"

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
		XMStoreFloat4x4( &m_World, XMMatrixIdentity() );
		m_position = XMFLOAT3( 0.0f, 0.0f, 0.0f );

		// Create vertex buffer
		HRESULT hr = m_vertexBuffer.Initialize( pDevice, vertices, ARRAYSIZE( vertices ) );
        COM_ERROR_IF_FAILED( hr, "Failed to create cube vertex buffer!" );

		// Create index buffer
        hr = m_indexBuffer.Initialize( pDevice, indices, ARRAYSIZE( indices ) );
        COM_ERROR_IF_FAILED( hr, "Failed to create cube index buffer!" );

		// Load and setup textures
		hr = CreateDDSTextureFromFile( pDevice, L"Resources\\Textures\\bricks_TEX.dds", nullptr, m_pTextureDiffuse.GetAddressOf() );
		COM_ERROR_IF_FAILED( hr, "Failed to create 'diffuse' texture!" );

		hr = CreateDDSTextureFromFile( pDevice, L"Resources\\Textures\\bricks_NORM.dds", nullptr, m_pTextureNormal.GetAddressOf() );
		COM_ERROR_IF_FAILED( hr, "Failed to create 'normal' texture!" );

		hr = CreateDDSTextureFromFile( pDevice, L"Resources\\Textures\\bricks_DISP.dds", nullptr, m_pTextureDisplacement.GetAddressOf() );
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

void Cube::Update( float dt )
{
	static float cummulativeTime = 0;
	cummulativeTime += dt;

	XMMATRIX mSpin = XMMatrixRotationY( cummulativeTime );
	XMMATRIX mTranslate = XMMatrixTranslation( m_position.x, m_position.y, m_position.z );
	XMMATRIX world = mTranslate;// * mSpin;
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

void Cube::UpdateBuffers( ConstantBuffer<Matrices>& cb_vs_matrices, Camera& pCamera )
{
	// Get the game object world transform
    XMMATRIX mGO = XMLoadFloat4x4( &m_World );
	cb_vs_matrices.data.mWorld = XMMatrixTranspose( mGO );

    // Store the view / projection in a constant buffer for the vertex shader to use
	cb_vs_matrices.data.mView = XMMatrixTranspose( pCamera.GetViewMatrix() );
	cb_vs_matrices.data.mProjection = XMMatrixTranspose( pCamera.GetProjectionMatrix() );
	if ( !cb_vs_matrices.ApplyChanges() ) return;
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

void Cube::DrawRTT( ID3D11DeviceContext* pContext )
{
	UINT offset = 0;
	pContext->IASetVertexBuffers( 0u, 1u, m_vertexBuffer.GetAddressOf(), m_vertexBuffer.StridePtr(), &offset );
	pContext->IASetIndexBuffer( m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0 );

	pContext->PSSetShaderResources( 0u, 1u, m_pTextureRTT.GetAddressOf() );
	pContext->PSSetShaderResources( 1u, 1u, m_pTextureNormal.GetAddressOf() );
	pContext->PSSetShaderResources( 2u, 1u, m_pTextureDisplacement.GetAddressOf() );

	pContext->DrawIndexed( m_indexBuffer.IndexCount(), 0u, 0u );
}

void Cube::SpawnControlWindow()
{
	if ( ImGui::Begin( "Material", FALSE, ImGuiWindowFlags_AlwaysAutoResize ) )
	{
		ImGui::Text( "Emission Color" );
		ImGui::SameLine();
		HelpMarker( COLOR_PICKER_HINT_TEXT );
		float emissive[] = { m_fEmissive.x , m_fEmissive.y, m_fEmissive.z, m_fEmissive.w };
		if ( ImGui::ColorEdit4( "##Emissive", emissive, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB ) )
			m_fEmissive = { emissive[0], emissive[1], emissive[2], emissive[3] };

		ImGui::Text( "Ambient Color" );
		ImGui::SameLine();
		HelpMarker( COLOR_PICKER_HINT_TEXT );
		float ambient[] = { m_fAmbient.x , m_fAmbient.y, m_fAmbient.z, m_fAmbient.w };
		if ( ImGui::ColorEdit4( "##Ambient", ambient, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB ) )
			m_fAmbient = { ambient[0], ambient[1], ambient[2], ambient[3] };

		ImGui::Text( "Diffuse Color" );
		ImGui::SameLine();
		HelpMarker( COLOR_PICKER_HINT_TEXT );
		float diffuse[] = { m_fDiffuse.x , m_fDiffuse.y, m_fDiffuse.z, m_fDiffuse.w };
		if ( ImGui::ColorEdit4( "##Diffuse", diffuse, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB ) )
			m_fDiffuse = { diffuse[0], diffuse[1], diffuse[2], diffuse[3] };

		ImGui::Text( "Specular Color" );
		ImGui::SameLine();
		HelpMarker( COLOR_PICKER_HINT_TEXT );
		float specular[] = { m_fSpecular.x , m_fSpecular.y, m_fSpecular.z, m_fSpecular.w };
		if ( ImGui::ColorEdit4( "##Specular", specular, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB ) )
			m_fSpecular = { specular[0], specular[1], specular[2], specular[3] };

		ImGui::Text( "Specular Power" );
		ImGui::SameLine();
		HelpMarker( SLIDER_HINT_TEXT );
		ImGui::SliderFloat( "##Spec Power", &m_fSpecularPower, 0.0f, 256.0f, "%1.f" );

		static bool useTexture = m_bUseTexture;
		ImGui::Checkbox( "Use Texture?", &useTexture );
		m_bUseTexture = useTexture;
	}
	ImGui::End();
}