#pragma once
#ifndef GAMEOBJECT3D_H
#define GAMEOBJECT3D_H

#include "GameObject.h"

class GameObject3D : public GameObject
{
public:
	void SetLookAtPos( XMFLOAT3 lookAtPos ) noexcept;
	void SetLookAtPos( float xPos, float yPos, float zPos ) noexcept;
	const XMVECTOR& GetForwardVector( bool omitY = false ) noexcept;
	const XMVECTOR& GetBackwardVector( bool omitY = false ) noexcept;
	const XMVECTOR& GetLeftVector( bool omitY = false ) noexcept;
	const XMVECTOR& GetRightVector( bool omitY = false ) noexcept;
	const XMVECTOR& GetUpVector() noexcept;
protected:
	virtual void UpdateMatrix();
	void UpdateDirectionVectors();
	const XMVECTOR DEFAULT_FORWARD_VECTOR = XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f );
	const XMVECTOR DEFAULT_UP_VECTOR = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
	const XMVECTOR DEFAULT_BACKWARD_VECTOR = XMVectorSet( 0.0f, 0.0f, -1.0f, 0.0f );
	const XMVECTOR DEFAULT_LEFT_VECTOR = XMVectorSet( -1.0f, 0.0f, 0.0f, 0.0f );
	const XMVECTOR DEFAULT_RIGHT_VECTOR = XMVectorSet( 1.0f, 0.0f, 0.0f, 0.0f );
	XMVECTOR vec_forward;
	XMVECTOR vec_left;
	XMVECTOR vec_right;
	XMVECTOR vec_backward;
	XMVECTOR vec_forward_noY;
	XMVECTOR vec_left_noY;
	XMVECTOR vec_right_noY;
	XMVECTOR vec_backward_noY;
};

#endif