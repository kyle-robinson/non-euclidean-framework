#pragma once
#ifndef CUBE_H
#define CUBE_H

#include "Resource.h"
#include "DDSTextureLoader.h"

#include "structures.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "ConstantBuffer.h"

struct Vertex
{
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT2 TexCoord;
	XMFLOAT3 Tangent;
	XMFLOAT3 Binormal;
};

class Cube
{
public:
	bool InitializeMesh( ID3D11Device* pDevice, ID3D11DeviceContext* pContext );
	void Update( float dt, ID3D11DeviceContext* pContext );
	void UpdateCB();
	void Draw( ID3D11DeviceContext* pContext );
	void SpawnControlWindow();

	inline ID3D11Buffer* const* GetMaterialCB() const noexcept { return m_cbMaterial.GetAddressOf(); }
	inline void SetPosition( DirectX::XMFLOAT3 position ) noexcept { m_position = position; }
	inline XMFLOAT4X4* GetTransform() noexcept { return &m_World; }

private:
	XMFLOAT4X4 m_World;
	XMFLOAT3 m_position;

	XMFLOAT4 m_fEmissive = { 0.0f, 0.0f, 0.0f, 1.0f };
	XMFLOAT4 m_fAmbient = { 0.1f, 0.1f, 0.1f, 1.0f };
	XMFLOAT4 m_fDiffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	XMFLOAT4 m_fSpecular = { 1.0f, 1.0f, 1.0f, 1.0f };
	FLOAT m_fSpecularPower = 128.0f;
	BOOL m_bUseTexture = TRUE;

	IndexBuffer m_indexBuffer;
	VertexBuffer<Vertex> m_vertexBuffer;
	ConstantBuffer<Material_CB> m_cbMaterial;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTextureDiffuse;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTextureNormal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTextureDisplacement;
};

#endif