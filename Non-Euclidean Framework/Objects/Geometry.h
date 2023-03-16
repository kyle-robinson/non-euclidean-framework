#pragma once
#ifndef GEOMETRY_H
#define GEOMETRY_H

class Camera;
#include "Resource.h"
#include "structures.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "ConstantBuffer.h"
#include "RenderableGameObject.h"
#include <dxtk/GeometricPrimitive.h>

class Geometry : public RenderableGameObject
{
public:
	bool CreateCylinder( ID3D11Device* pDevice, ID3D11DeviceContext* pContext );
	bool CreateCone( ID3D11Device* pDevice, ID3D11DeviceContext* pContext );
	bool CreateDodecahedron( ID3D11Device* pDevice, ID3D11DeviceContext* pContext );
	bool CreateIcosahedron( ID3D11Device* pDevice, ID3D11DeviceContext* pContext );
	bool CreateOctahedron( ID3D11Device* pDevice, ID3D11DeviceContext* pContext );
	bool CreateTeapot( ID3D11Device* pDevice, ID3D11DeviceContext* pContext );

	void Update( float dt );
	void UpdateBuffers( ID3D11DeviceContext* pContext, ConstantBuffer<Matrices>& cb_vs_matrices, Camera& camera );
	void Draw( ID3D11DeviceContext* pContext );
	inline void SetTexture( ID3D11ShaderResourceView* texture ) noexcept { m_pTexture = texture; }

private:
	bool InitializeMesh( ID3D11Device* pDevice, ID3D11DeviceContext* pContext, std::wstring texPath );

	IndexBuffer m_indexBuffer;
	ConstantBuffer<Material_CB> m_cbMaterial;
	VertexBuffer<GeometricPrimitive::VertexType> m_vertexBuffer;

	std::vector<uint16_t> indices;
	std::vector<GeometricPrimitive::VertexType> vertices;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTexture;
};

#endif