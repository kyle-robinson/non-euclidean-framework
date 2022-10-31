#include "stdafx.h"
#include "Mesh.h"

Mesh::Mesh( ID3D11Device* device,
	ID3D11DeviceContext* context,
	std::vector<VertexOBJ>& vertices,
	std::vector<WORD>& indices,
	std::vector<Texture>& textures,
	const DirectX::XMMATRIX& transformMatrix )
{
	try
	{
		this->context = context;
		this->textures = textures;
		this->transformMatrix = transformMatrix;

		HRESULT hr = vertexBuffer.Initialize( device, vertices.data(), vertices.size() );
		COM_ERROR_IF_FAILED( hr, "Failed to initialize vertex buffer for mesh!" );

		hr = indexBuffer.Initialize( device, indices.data(), indices.size() );
		COM_ERROR_IF_FAILED( hr, "Failed to initialize index buffer for mesh!" );
	}
	catch ( COMException& exception )
	{
		ErrorLogger::Log( exception );
		return;
	}
}

const DirectX::XMMATRIX& Mesh::GetTransformMatrix()
{
	return transformMatrix;
}

Mesh::Mesh( const Mesh& mesh )
{
	context = mesh.context;
	vertexBuffer = mesh.vertexBuffer;
	indexBuffer = mesh.indexBuffer;
	textures = mesh.textures;
	transformMatrix = mesh.transformMatrix;
}

void Mesh::Draw()
{
	for ( int i = 0; i < textures.size(); i++ )
	{
		if ( textures[i].GetType() == aiTextureType_DIFFUSE )
		{
			context->PSSetShaderResources( 0, 1, textures[i].GetTextureResourceViewAddress() );
			break;
		}
		if ( textures[i].GetType() == aiTextureType_SPECULAR )
		{
			context->PSSetShaderResources( 1, 1, textures[i].GetTextureResourceViewAddress() );
			break;
		}
	}

	UINT offset = 0;
	context->IASetVertexBuffers( 0, 1, vertexBuffer.GetAddressOf(), vertexBuffer.StridePtr(), &offset );
	context->IASetIndexBuffer( indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0 );
	context->DrawIndexed( indexBuffer.IndexCount(), 0, 0 );
}