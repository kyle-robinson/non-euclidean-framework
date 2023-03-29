#include "stdafx.h"
#include "Camera.h"
#include "MousePicking.h"
#include "GameObject3D.h"
#include <DirectXCollision.h>

void MousePicking::Initialize( int width, int height )
{
	this->width = width;
	this->height = height;
}

void MousePicking::UpdateMatrices( std::unique_ptr<Camera>& camera )
{
	this->viewMatrix = camera->GetViewMatrix();
	this->projectionMatrix = camera->GetProjectionMatrix();
}

bool MousePicking::TestIntersection( int mouseX, int mouseY, GameObject3D object )
{
	XMMATRIX invProj = XMMatrixInverse( nullptr, projectionMatrix );
	XMMATRIX invView = XMMatrixInverse( nullptr, viewMatrix );

	// Move the mouse cursor coordinates into the -1 to +1 range
	float pointX = ( 2.0f * static_cast<float>( mouseX ) ) / static_cast<float>( width ) - 1.0f;
	float pointY = 1.0f - ( 2.0f * static_cast<float>( mouseY ) ) / static_cast<float>( height );

	XMVECTOR eyePos, dummy;
	XMMatrixDecompose( &dummy, &dummy, &eyePos, invView );

	// Transform the mouse position into world space
	XMVECTOR rayOri = XMVectorSet( pointX, pointY, 0.0f, 0.0f );
	rayOri = XMVector3Transform( rayOri, invProj );
	rayOri = XMVector3Transform( rayOri, invView );

	XMVECTOR rayDir = rayOri - eyePos;
	rayDir = XMVector3Normalize( rayDir );

	// Generate bounding sphere
	BoundingSphere bs;
	bs.Center = object.GetPositionFloat3();

	float maxRadius = 0.0f;
	if ( fabs( object.GetScaleFloat3().x > maxRadius ) )
		maxRadius = fabs( object.GetScaleFloat3().x );
	if ( fabs( object.GetScaleFloat3().y > maxRadius ) )
		maxRadius = fabs( object.GetScaleFloat3().y );
	if ( fabs( object.GetScaleFloat3().z > maxRadius ) )
		maxRadius = fabs( object.GetScaleFloat3().z );
	bs.Radius = maxRadius * 0.75f;

	// Check for intersection between sphere and ray
	float distance = 1000.0f;
	if ( bs.Intersects( rayOri, rayDir, distance ) )
		return true;

	return false;
}