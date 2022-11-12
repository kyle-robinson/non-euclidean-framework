#pragma once
#ifndef STENCILCUBE_H
#define STENCILCUBE_H

#include "Face.h"

class StencilCube
{
public:
	bool Initialize( ID3D11DeviceContext* pContext, ID3D11Device* pDevice );
	void SetTexture( Side side, ID3D11ShaderResourceView* pTexture ) noexcept;
	std::unordered_map<Side, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> GetTextures() const noexcept;
	void SetRotation( float x, float y, float z ) noexcept;
	void SetScale( float x, float y, float z ) noexcept;
	void Draw( ID3D11DeviceContext* pContext, ConstantBuffer<Matrices>& cb_vs_matrix, Camera& pCamera );
private:
	std::unordered_map<Side, std::shared_ptr<Face>> m_pFaces;
	std::unordered_map<Side, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_pTextures;
};

#endif