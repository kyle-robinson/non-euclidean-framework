#pragma once
#ifndef STENCILCUBE_H
#define STENCILCUBE_H

#include "Face.h"

class StencilCube
{
public:
	bool Initialize( ID3D11DeviceContext* pContext, ID3D11Device* pDevice );
	void SetTexture( Side side, ID3D11ShaderResourceView* pTexture ) noexcept;
	void SetPosition( float x, float y, float z ) noexcept;
	inline XMFLOAT3 GetPosition() const noexcept { return position; }
	void SetScale( float x, float y, float z ) noexcept;
	inline std::shared_ptr<Face> GetFace( Side side ) noexcept { return m_pFaces[side]; }
	void Draw( ID3D11DeviceContext* pContext, ConstantBuffer<Matrices>& cb_vs_matrix, Camera& pCamera );
	void DrawFace( Side side, ConstantBuffer<Matrices>& cb_vs_matrix, Camera& pCamera );
private:
	XMFLOAT3 position;
	std::unordered_map<Side, std::shared_ptr<Face>> m_pFaces;
	std::unordered_map<Side, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_pTextures;
};

#endif