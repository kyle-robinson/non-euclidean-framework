#pragma once
#ifndef GEOMETRY_H
#define GEOMETRY_H

class Camera;
#include "Resource.h"
#include "structures.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "ConstantBuffer.h"
#include <dxtk/GeometricPrimitive.h>

class Geometry
{
public:
	bool CreateCylinder( ID3D11Device* pDevice, ID3D11DeviceContext* pContext );
	bool CreateCone( ID3D11Device* pDevice, ID3D11DeviceContext* pContext );
	bool CreateDodecahedron( ID3D11Device* pDevice, ID3D11DeviceContext* pContext );
	bool CreateIcosahedron( ID3D11Device* pDevice, ID3D11DeviceContext* pContext );
	bool CreateOctahedron( ID3D11Device* pDevice, ID3D11DeviceContext* pContext );
	bool CreateTeapot( ID3D11Device* pDevice, ID3D11DeviceContext* pContext );

	void Update( float dt );
	void UpdateCB();
	void UpdateBuffers( ConstantBuffer<Matrices>& cb_vs_matrices, Camera& camera );
	void Draw( ID3D11DeviceContext* pContext );

	inline void SetTexture( ID3D11ShaderResourceView* texture ) noexcept { m_pTexture = texture; }
	inline ID3D11Buffer* const* GetCB() const noexcept { return m_cbMaterial.GetAddressOf(); }
	inline void SetPosition( DirectX::XMFLOAT3 position ) noexcept { m_position = position; }
	inline XMFLOAT4X4* GetTransform() noexcept { return &m_World; }

private:
	bool InitializeMesh( ID3D11Device* pDevice, ID3D11DeviceContext* pContext, std::wstring texPath );
	XMFLOAT4X4 m_World;
	XMFLOAT3 m_position;


	XMFLOAT4 m_fEmissive = { 0.0f, 0.0f, 0.0f, 1.0f };
	XMFLOAT4 m_fAmbient = { 0.1f, 0.1f, 0.1f, 1.0f };
	XMFLOAT4 m_fDiffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	XMFLOAT4 m_fSpecular = { 1.0f, 1.0f, 1.0f, 1.0f };
	FLOAT m_fSpecularPower = 128.0f;
	BOOL m_bUseTexture = TRUE;

	IndexBuffer m_indexBuffer;
	ConstantBuffer<Material_CB> m_cbMaterial;
	VertexBuffer<GeometricPrimitive::VertexType> m_vertexBuffer;

	std::vector<uint16_t> indices;
	std::vector<GeometricPrimitive::VertexType> vertices;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTexture;
};

#endif