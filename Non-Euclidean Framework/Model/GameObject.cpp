#include "stdafx.h"
#include "GameObject.h"

const XMVECTOR& GameObject::GetPositionVector() const noexcept
{
	return posVector;
}

const XMFLOAT3& GameObject::GetPositionFloat3() const noexcept
{
	return position;
}

const XMVECTOR& GameObject::GetRotationVector() const noexcept
{
	return rotVector;
}

const XMFLOAT3& GameObject::GetRotationFloat3() const noexcept
{
	return rotation;
}

const XMFLOAT3& GameObject::GetScaleFloat3() const noexcept
{
	return { scale.x, scale.y, scale.z };
}

const std::string& GameObject::GetModelName() const noexcept
{
	return modelName;
}

void GameObject::SetModelName( const std::string& name ) noexcept
{
	modelName = name;
}

/// POSITIONS
void GameObject::SetInitialPosition( const XMFLOAT3& pos ) noexcept
{
	initialPos = pos;
	SetPosition( pos );
}

void GameObject::SetInitialPosition( float xPos, float yPos, float zPos ) noexcept
{
	SetInitialPosition( { xPos, yPos, zPos } );
}

void GameObject::SetPosition( const XMVECTOR& pos ) noexcept
{
	XMStoreFloat3( &position, pos );
	posVector = pos;
	UpdateMatrix();
}

void GameObject::SetPosition( const XMFLOAT3& pos ) noexcept
{
	position = pos;
	posVector = XMLoadFloat3( &position );
	UpdateMatrix();
}

void GameObject::SetPosition( float xPos, float yPos, float zPos ) noexcept
{
	SetPosition( XMFLOAT3( xPos, yPos, zPos ) );
}

void GameObject::AdjustPosition( const XMVECTOR& pos ) noexcept
{
	posVector += pos;
	XMStoreFloat3( &position, posVector );
	UpdateMatrix();
}

void GameObject::AdjustPosition( const XMFLOAT3& pos ) noexcept
{
	position.x += pos.x;
	position.y += pos.y;
	position.z += pos.z;
	posVector = XMLoadFloat3( &position );
	UpdateMatrix();
}

void GameObject::AdjustPosition( float xPos, float yPos, float zPos ) noexcept
{
	AdjustPosition( XMFLOAT3( xPos, yPos, zPos ) );
}

void GameObject::ResetPosition() noexcept
{
	position = initialPos;
	UpdateMatrix();
}

/// ROTATIONS
void GameObject::SetInitialRotation( const XMFLOAT3& rot ) noexcept
{
	initialRot = rot;
	SetRotation( rot );
}

void GameObject::SetInitialRotation( float xRot, float yRot, float zRot ) noexcept
{
	SetInitialRotation( { xRot, yRot, zRot } );
}

void GameObject::SetRotation( const XMVECTOR& rot ) noexcept
{
	rotVector = rot;
	XMStoreFloat3( &rotation, rot );
	UpdateMatrix();
}

void GameObject::SetRotation( const XMFLOAT3& rot ) noexcept
{
	rotation = rot;
	rotVector = XMLoadFloat3( &rotation );
	UpdateMatrix();
}

void GameObject::SetRotation( float xRot, float yRot, float zRot ) noexcept
{
	SetRotation( XMFLOAT3( xRot, yRot, zRot ) );
}

void GameObject::AdjustRotation( const XMVECTOR& rot ) noexcept
{
	rotVector += rot;
	XMStoreFloat3( &rotation, rotVector );
	UpdateMatrix();
}

void GameObject::AdjustRotation( const XMFLOAT3& rot ) noexcept
{
	rotation.x += rot.x;
	rotation.y += rot.y;
	rotation.z += rot.z;

	if ( rotation.x >= XMConvertToRadians( 90.0f ) )
		rotation.x = XMConvertToRadians( 90.0f );

	if ( rotation.x <= XMConvertToRadians( -90.0f ) )
		rotation.x = XMConvertToRadians( -90.0f );

	rotVector = XMLoadFloat3( &rotation );
	UpdateMatrix();
}

void GameObject::AdjustRotation( float xRot, float yRot, float zRot ) noexcept
{
	AdjustRotation( XMFLOAT3( xRot, yRot, zRot ) );
}

void GameObject::ResetRotation() noexcept
{
	rotation = initialRot;
	UpdateMatrix();
}

/// SCALE
void GameObject::SetInitialScale( float xScale, float yScale, float zScale ) noexcept
{
	initialScale = { xScale, yScale, zScale };
	SetScale( xScale, yScale, zScale );
}

void GameObject::SetScale( float xScale, float yScale, float zScale ) noexcept
{
	scale = { xScale, yScale, zScale };
	UpdateMatrix();
}

void GameObject::AdjustScale( float xScale, float yScale, float zScale ) noexcept
{
	scale.x += xScale;
	scale.y += yScale;
	scale.z += zScale;
	UpdateMatrix();
}

void GameObject::ResetScale() noexcept
{
	scale = initialScale;
	UpdateMatrix();
}

void GameObject::UpdateMatrix()
{
	assert( "UpdateMatrix must be overridden!" && 0 );
}