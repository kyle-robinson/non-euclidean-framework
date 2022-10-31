#pragma once
#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Model.h"

class GameObject
{
public:
	const XMVECTOR& GetPositionVector() const noexcept;
	const XMFLOAT3& GetPositionFloat3() const noexcept;
	const XMVECTOR& GetRotationVector() const noexcept;
	const XMFLOAT3& GetRotationFloat3() const noexcept;
	const XMFLOAT3& GetScaleFloat3() const noexcept;
	
	const std::string& GetModelName() const noexcept;
	void SetModelName( const std::string& name ) noexcept;

	void SetInitialPosition( const XMFLOAT3& pos ) noexcept;
	void SetInitialPosition( float xPos, float yPos, float zPos ) noexcept;
	void SetPosition( const XMVECTOR& pos ) noexcept;
	void SetPosition( const XMFLOAT3& pos ) noexcept;
	void SetPosition( float xPos, float yPos, float zPos ) noexcept;
	void AdjustPosition( const XMVECTOR& pos ) noexcept;
	void AdjustPosition( const XMFLOAT3& pos ) noexcept;
	void AdjustPosition( float xPos, float yPos, float zPos ) noexcept;
	void ResetPosition() noexcept;

	void SetInitialRotation( const XMFLOAT3& rot ) noexcept;
	void SetInitialRotation( float xRot, float yRot, float zRot ) noexcept;
	void SetRotation( const XMVECTOR& rot ) noexcept;
	void SetRotation( const XMFLOAT3& rot ) noexcept;
	void SetRotation( float xRot, float yRot, float zRot ) noexcept;
	void AdjustRotation( const XMVECTOR& rot ) noexcept;
	void AdjustRotation( const XMFLOAT3& rot ) noexcept;
	void AdjustRotation( float xRot, float yRot, float zRot ) noexcept;
	void ResetRotation() noexcept;

	void SetInitialScale( float xScale, float yScale, float zScale = 1.0f ) noexcept;
	void SetScale( float xScale, float yScale, float zScale = 1.0f ) noexcept;
	void AdjustScale( float xScale, float yScale, float zScale = 1.0f ) noexcept;
	void ResetScale() noexcept;
protected:
	virtual void UpdateMatrix();
	std::string modelName;
	XMFLOAT3 initialPos, initialRot, initialScale;
	XMVECTOR posVector, rotVector;
	XMFLOAT3 position, rotation;
	XMFLOAT3 scale;
};

#endif