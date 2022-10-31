#include "stdafx.h"
#include "Camera.h"

void Camera::Initialize( const XMFLOAT3& initialPosition, int width, int height )
{
	position = initialPosition;
	posVector = XMLoadFloat3( &position );
	rotation = { 0.0f, 0.0f, 0.0f };
	rotVector = XMLoadFloat3( &rotation );
	UpdateMatrix();
	SetProjectionValues( 75.0f, static_cast<float>( width ) / static_cast<float>( height ), 0.01f, 100.0f );
}

void Camera::SetProjectionValues( FLOAT fovDegrees, FLOAT aspectRatio, FLOAT nearZ, FLOAT farZ )
{
	this->fovDegrees = fovDegrees;
	this->nearZ = nearZ;
	this->farZ = farZ;
	float fovRadians = ( fovDegrees / 360.0f ) * XM_2PI;
	projection = XMMatrixPerspectiveFovLH( fovRadians, aspectRatio, nearZ, farZ );
}

// DIRECTION VECTORS
const XMVECTOR& Camera::GetForwardVector( BOOL omitY ) noexcept
{
	return omitY ? vec_forward_noY : vec_forward;
}

const XMVECTOR& Camera::GetBackwardVector( BOOL omitY ) noexcept
{
	return omitY ? vec_backward_noY : vec_backward;
}

const XMVECTOR& Camera::GetLeftVector( BOOL omitY ) noexcept
{
	return omitY ? vec_left_noY : vec_left;
}

const XMVECTOR& Camera::GetRightVector( BOOL omitY ) noexcept
{
	return omitY ? vec_right_noY : vec_right;
}

const XMVECTOR& Camera::GetUpVector() noexcept
{
	return DEFAULT_UP_VECTOR;
}

// CAMERA MOVEMENT
void Camera::MoveForward( FLOAT dt ) noexcept
{
	AdjustPosition( GetForwardVector() * GetCameraSpeed() * dt );
}

void Camera::MoveBackward( FLOAT dt ) noexcept
{
	AdjustPosition( GetBackwardVector() * GetCameraSpeed() * dt );
}

void Camera::MoveLeft( FLOAT dt ) noexcept
{
	AdjustPosition( GetLeftVector() * GetCameraSpeed() * dt );
}

void Camera::MoveRight( FLOAT dt ) noexcept
{
	AdjustPosition( GetRightVector() * GetCameraSpeed() * dt );
}

void Camera::MoveUp( FLOAT dt ) noexcept
{
	AdjustPosition( 0.0f, GetCameraSpeed() * dt, 0.0f );
}

void Camera::MoveDown( FLOAT dt ) noexcept
{
	AdjustPosition( 0.0f, -GetCameraSpeed() * dt, 0.0f );
}

// CAMERA POSITION
void Camera::SetPosition( FLOAT x, FLOAT y, FLOAT z ) noexcept
{
	SetPosition( XMFLOAT3( x, y, z ) );
}

void Camera::SetPosition( const XMVECTOR& pos ) noexcept
{
	XMStoreFloat3( &position, pos );
	posVector = pos;
	UpdateMatrix();
}

void Camera::SetPosition( const XMFLOAT3& pos ) noexcept
{
	position = pos;
	posVector = XMLoadFloat3( &position );
	UpdateMatrix();
}

void Camera::AdjustPosition( FLOAT x, FLOAT y, FLOAT z ) noexcept
{
	AdjustPosition( XMFLOAT3( x, y, z ) );
}

void Camera::AdjustPosition( const XMVECTOR& pos ) noexcept
{
	posVector += pos;
	XMStoreFloat3( &position, posVector );
	UpdateMatrix();
}

void Camera::AdjustPosition( const XMFLOAT3& pos ) noexcept
{
	position.x += pos.x;
	position.y += pos.y;
	position.z += pos.z;
	posVector = XMLoadFloat3( &position );
	UpdateMatrix();
}

// CAMERA ROTATION
void Camera::SetRotation( FLOAT x, FLOAT y, FLOAT z ) noexcept
{
	SetRotation( XMFLOAT3( x, y, z ) );
}

void Camera::SetRotation( const XMVECTOR& rot ) noexcept
{
	rotVector = rot;
	XMStoreFloat3( &rotation, rot );
	UpdateMatrix();
}

void Camera::SetRotation( const XMFLOAT3& rot ) noexcept
{
	rotation = rot;
	rotVector = XMLoadFloat3( &rotation );
	UpdateMatrix();
}

void Camera::AdjustRotation( FLOAT x, FLOAT y, FLOAT z ) noexcept
{
	AdjustRotation( XMFLOAT3( x, y, z ) );
}

void Camera::AdjustRotation( const XMVECTOR& rot ) noexcept
{
	rotVector += rot;
	XMStoreFloat3( &rotation, rotVector );
	UpdateMatrix();
}

void Camera::AdjustRotation( const XMFLOAT3& rot ) noexcept
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

// UPDATE MATRICES
void Camera::UpdateMatrix()
{
	// Update Camera Target
	XMMATRIX cameraRotation = XMMatrixRotationRollPitchYaw( rotation.x, rotation.y, rotation.z );
	XMVECTOR camTarget = XMVector3TransformCoord( DEFAULT_FORWARD_VECTOR, cameraRotation );
	camTarget += posVector;

	// Store camera target
	cameraTarget = { XMVectorGetX( camTarget ), XMVectorGetY( camTarget ), XMVectorGetZ( camTarget ) };

	XMVECTOR upDir = XMVector3TransformCoord( DEFAULT_UP_VECTOR, cameraRotation );
	view = XMMatrixLookAtLH( posVector, camTarget, upDir );

	UpdateDirectionVectors();
}

void Camera::UpdateDirectionVectors()
{
	XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw( rotation.x, rotation.y, 0.0f );
	vec_forward = XMVector3TransformCoord( DEFAULT_FORWARD_VECTOR, vecRotationMatrix );
	vec_backward = XMVector3TransformCoord( DEFAULT_BACKWARD_VECTOR, vecRotationMatrix );
	vec_left = XMVector3TransformCoord( DEFAULT_LEFT_VECTOR, vecRotationMatrix );
	vec_right = XMVector3TransformCoord( DEFAULT_RIGHT_VECTOR, vecRotationMatrix );

	XMMATRIX vecRotationMatrix_noY = XMMatrixRotationRollPitchYaw( 0.0f, rotation.y, 0.0f );
	vec_forward_noY = XMVector3TransformCoord( DEFAULT_FORWARD_VECTOR, vecRotationMatrix_noY );
	vec_backward_noY = XMVector3TransformCoord( DEFAULT_BACKWARD_VECTOR, vecRotationMatrix_noY );
	vec_left_noY = XMVector3TransformCoord( DEFAULT_LEFT_VECTOR, vecRotationMatrix_noY );
	vec_right_noY = XMVector3TransformCoord( DEFAULT_RIGHT_VECTOR, vecRotationMatrix_noY );
}