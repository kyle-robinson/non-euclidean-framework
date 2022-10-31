#include "stdafx.h"
#include "RenderableGameObject.h"

bool RenderableGameObject::Initialize(
	const std::string& filePath,
	ID3D11Device* device,
	ID3D11DeviceContext* context,
	ConstantBuffer<Matrices>& cb_vs_vertexshader )
{
	if ( !model.Initialize( filePath, device, context, cb_vs_vertexshader ) )
		return false;

	SetPosition( XMFLOAT3( 0.0f, 0.0f, 0.0f ) );
	SetRotation( XMFLOAT3( 0.0f, 0.0f, 0.0f ) );
	UpdateMatrix();
	return true;
}

void RenderableGameObject::Draw( const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix )
{
	model.Draw( worldMatrix, viewMatrix, projectionMatrix );
}

void RenderableGameObject::UpdateMatrix()
{
	worldMatrix = XMMatrixScaling( scale.x, scale.y, scale.y ) *
		XMMatrixRotationRollPitchYaw( rotation.x, rotation.y, rotation.z ) *
		XMMatrixTranslation( position.x, position.y, position.z );
	UpdateDirectionVectors();
}