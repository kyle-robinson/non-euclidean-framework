#pragma once
#ifndef STENCILCUBE_H
#define STENCILCUBE_H

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

class StencilCube
{
public:
	bool Initialize( ID3D11DeviceContext* pContext, ID3D11Device* pDevice );
	void SetTexture( Side side, ID3D11ShaderResourceView* pTexture );
	void Draw( ID3D11DeviceContext* pContext, ConstantBuffer<Matrices>& cb_vs_matrix, Camera& pCamera );
private:
	std::unordered_map<Side, std::shared_ptr<Face>> m_pFaces;
	std::unordered_map<Side, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_pTextures;
};

#endif