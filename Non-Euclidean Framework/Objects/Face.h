#pragma once
#ifndef FACE_H
#define FACE_H

class Camera;
#if _x64
#include "RenderableGameObject.h"
#else
#include "structures.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "ConstantBuffer.h"

struct VertexOBJ
{
	XMFLOAT3 Position;
	XMFLOAT2 TexCoord;
	XMFLOAT3 Normal;
};
#endif

enum class Side
{
	FRONT,
	BACK,
	LEFT,
	RIGHT,
	TOP,
	BOTTOM
};

class Face
#if _x64
	: public RenderableGameObject
#endif
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