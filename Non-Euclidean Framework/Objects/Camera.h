#pragma once
#ifndef CAMERA_H
#define CAMERA_H

using namespace DirectX;

class Camera
{
public:
	void Initialize( const XMFLOAT3& initialPosition, int width, int height );
	void SetProjectionValues( FLOAT fovDegrees,FLOAT aspectRatio, FLOAT nearZ, FLOAT farZ  );

	inline const XMMATRIX& GetViewMatrix() const noexcept { return view; };
	inline const XMMATRIX& GetProjectionMatrix() const noexcept { return projection; };
	inline const XMMATRIX& GetViewProjectionMatrix() const noexcept { return view * projection; };

	inline const XMFLOAT3& GetPositionFloat3() const noexcept { return position; };
	inline const XMVECTOR& GetPositionVector() const noexcept { return posVector; }
	inline const XMFLOAT3& GetRotationFloat3() const noexcept { return rotation; };
	inline const XMVECTOR& GetRotationVector() const noexcept { return rotVector; };
	inline const XMFLOAT3& GetCameraTarget() const noexcept { return cameraTarget; }

	const XMVECTOR& GetForwardVector( BOOL omitY = true ) noexcept;
	const XMVECTOR& GetBackwardVector( BOOL omitY = true ) noexcept;
	const XMVECTOR& GetLeftVector( BOOL omitY = true ) noexcept;
	const XMVECTOR& GetRightVector( BOOL omitY = true ) noexcept;
	const XMVECTOR& GetUpVector() noexcept;

	void SetPosition( const XMFLOAT3& pos ) noexcept;
	void SetPosition( const XMVECTOR& pos ) noexcept;
	void SetPosition( FLOAT x, FLOAT y, FLOAT z ) noexcept;
	void AdjustPosition( const XMFLOAT3& pos ) noexcept;
	void AdjustPosition( const XMVECTOR& pos ) noexcept;
	void AdjustPosition( FLOAT x, FLOAT y, FLOAT z ) noexcept;

	void SetRotation( const XMFLOAT3& rot ) noexcept;
	void SetRotation( const XMVECTOR& rot ) noexcept;
	void SetRotation( FLOAT x, FLOAT y, FLOAT z ) noexcept;
	void AdjustRotation( const XMFLOAT3& rot ) noexcept;
	void AdjustRotation( const XMVECTOR& rot ) noexcept;
	void AdjustRotation( FLOAT x, FLOAT y, FLOAT z ) noexcept;

	inline const float& GetFoVDegrees() const noexcept { return fovDegrees; }
	inline const float& GetNearZ() const noexcept { return nearZ; }
	inline const float& GetFarZ() const noexcept { return farZ; }

	void UpdateMatrix();

	// CAMERA MOVEMENT
	inline const FLOAT& GetCameraSpeed() const noexcept { return cameraSpeed; };
	inline void SetCameraSpeed( FLOAT newSpeed ) noexcept { cameraSpeed = newSpeed; };
	inline void UpdateCameraSpeed( FLOAT updateSpeed ) noexcept { cameraSpeed += updateSpeed; };

	void MoveForward( FLOAT dt ) noexcept;
	void MoveBackward( FLOAT dt ) noexcept;
	void MoveLeft( FLOAT dt ) noexcept;
	void MoveRight( FLOAT dt ) noexcept;
	void MoveUp( FLOAT dt ) noexcept;
	void MoveDown( FLOAT dt ) noexcept;

private:
	void UpdateDirectionVectors();
	const XMVECTOR DEFAULT_FORWARD_VECTOR = XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f );
	const XMVECTOR DEFAULT_UP_VECTOR = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
	const XMVECTOR DEFAULT_BACKWARD_VECTOR = XMVectorSet( 0.0f, 0.0f, -1.0f, 0.0f );
	const XMVECTOR DEFAULT_LEFT_VECTOR = XMVectorSet( -1.0f, 0.0f, 0.0f, 0.0f );
	const XMVECTOR DEFAULT_RIGHT_VECTOR = XMVectorSet( 1.0f, 0.0f, 0.0f, 0.0f );
	XMVECTOR vec_forward, vec_left, vec_right, vec_backward;
	XMVECTOR vec_forward_noY, vec_left_noY, vec_right_noY, vec_backward_noY;

	XMMATRIX view, projection;
	XMVECTOR posVector, rotVector;
	XMFLOAT3 position, rotation, cameraTarget;
	FLOAT fovDegrees, aspectRatio, nearZ, farZ, cameraSpeed;
};

#endif