#include "stdafx.h"
#include "GameObject3D.h"

void GameObject3D::SetLookAtPos( XMFLOAT3 lookAtPos ) noexcept
{
	if ( lookAtPos.x == position.x &&
		 lookAtPos.y == position.y &&
		 lookAtPos.z == position.z ) return;

	lookAtPos.x = position.x - lookAtPos.x;
	lookAtPos.y = position.y - lookAtPos.y;
	lookAtPos.z = position.z - lookAtPos.z;

	float pitch = 0.0f;
	if ( lookAtPos.y != 0.0f )
	{
		const float distance = sqrt( lookAtPos.x * lookAtPos.x + lookAtPos.z * lookAtPos.z );
		pitch = atan( lookAtPos.y / distance );
	}

	float yaw = 0.0f;
	if ( lookAtPos.x != 0.0f )
		yaw = atan( lookAtPos.x / lookAtPos.z );
	if ( lookAtPos.z > 0.0f )
		yaw += XM_PI;

	SetRotation( pitch, yaw, 0.0f );
}

void GameObject3D::SetLookAtPos( float xPos, float yPos, float zPos ) noexcept
{
	SetLookAtPos( { xPos, yPos, zPos } );
}

const XMVECTOR& GameObject3D::GetForwardVector( bool omitY ) noexcept
{
	return omitY ? vec_forward_noY : vec_forward;
}

const XMVECTOR& GameObject3D::GetBackwardVector( bool omitY ) noexcept
{
	return omitY ? vec_backward_noY : vec_backward;
}

const XMVECTOR& GameObject3D::GetLeftVector( bool omitY ) noexcept
{
	return omitY ? vec_left_noY : vec_left;
}

const XMVECTOR& GameObject3D::GetRightVector( bool omitY ) noexcept
{
	return omitY ? vec_right_noY : vec_right;
}

const XMVECTOR& GameObject3D::GetUpVector() noexcept
{
	return DEFAULT_UP_VECTOR;
}

void GameObject3D::UpdateMatrix()
{
	assert( "UpdateMatrix must be overridden!" && 0 );
}

void GameObject3D::UpdateDirectionVectors()
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