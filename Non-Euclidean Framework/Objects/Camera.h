#pragma once
#ifndef CAMERA_H
#define CAMERA_H

using namespace DirectX;

class Camera
{
public:
	void Initialize( const XMFLOAT3& initialPosition, int width, int height );
	void SetProjectionValues( FLOAT fovDegrees,FLOAT aspectRatio, FLOAT nearZ, FLOAT farZ  );
	Camera& operator = ( const Camera& cam ) { return *this; }

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

	inline const float& GetAspectRatio() const noexcept { return aspectRatio; }
	inline const float& GetFoVDegrees() const noexcept { return fovDegrees; }
	inline const float& GetNearZ() const noexcept { return nearZ; }
	inline const float& GetFarZ() const noexcept { return farZ; }

	inline void SetAspectRatio( FLOAT aspect ) noexcept { aspectRatio = aspect; UpdateProjectionValues(); }
	inline void SetFovDegrees( FLOAT fov ) noexcept { fovDegrees = fov; UpdateProjectionValues(); }
	inline void SetNearZ( FLOAT nearPlane ) noexcept { nearZ = nearPlane; UpdateProjectionValues(); }
	inline void SetFarZ( FLOAT farPlane ) noexcept { farZ = farPlane; UpdateProjectionValues(); }

	void UpdateMatrix();

	// CAMERA MOVEMENT
	inline const FLOAT& GetCameraSpeed() const noexcept { return cameraSpeed; };
	inline void SetCameraSpeed( FLOAT newSpeed ) noexcept { cameraSpeed = newSpeed; };
	inline void UpdateCameraSpeed( FLOAT updateSpeed ) noexcept { cameraSpeed += updateSpeed; };

	void MoveForward( FLOAT dt, BOOL omitY = true ) noexcept;
	void MoveBackward( FLOAT dt, BOOL omitY = true ) noexcept;
	void MoveLeft( FLOAT dt, BOOL omitY = true ) noexcept;
	void MoveRight( FLOAT dt, BOOL omitY = true ) noexcept;
	void MoveUp( FLOAT dt ) noexcept;
	void MoveDown( FLOAT dt ) noexcept;

	inline bool CanCollide() const noexcept { return m_bCollisions; }
	inline void EnableCollisions() noexcept { m_bCollisions = true; }
	inline void DisableCollisions() noexcept { m_bCollisions = false; }

private:
	void UpdateProjectionValues();
	void UpdateDirectionVectors();

	const XMVECTOR DEFAULT_FORWARD_VECTOR = XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f );
	const XMVECTOR DEFAULT_UP_VECTOR = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
	const XMVECTOR DEFAULT_BACKWARD_VECTOR = XMVectorSet( 0.0f, 0.0f, -1.0f, 0.0f );
	const XMVECTOR DEFAULT_LEFT_VECTOR = XMVectorSet( -1.0f, 0.0f, 0.0f, 0.0f );
	const XMVECTOR DEFAULT_RIGHT_VECTOR = XMVectorSet( 1.0f, 0.0f, 0.0f, 0.0f );
	XMVECTOR vec_forward, vec_left, vec_right, vec_backward;
	XMVECTOR vec_forward_noY, vec_left_noY, vec_right_noY, vec_backward_noY;

	bool m_bCollisions = true;
	XMMATRIX view, projection;
	XMVECTOR posVector, rotVector;
	XMFLOAT3 position, rotation, cameraTarget;
	FLOAT fovDegrees, aspectRatio, nearZ, farZ, cameraSpeed;
};

#endif