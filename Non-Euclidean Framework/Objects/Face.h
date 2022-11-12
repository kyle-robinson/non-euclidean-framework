#pragma once
#ifndef FACE_H
#define FACE_H

class Camera;
#include "RenderableGameObject.h"

enum class Side
{
	FRONT,
	BACK,
	LEFT,
	RIGHT,
	TOP,
	BOTTOM
};

class Face : public RenderableGameObject
{
public:
	bool Initialize( ID3D11DeviceContext* pContext, ID3D11Device* pDevice );
	void Draw( ConstantBuffer<Matrices>& cb_vs_matrix, Camera& pCamera ) noexcept;
private:
	ID3D11DeviceContext* context;
	VertexBuffer<VertexOBJ> vb_plane;
	IndexBuffer ib_plane;
};

#endif