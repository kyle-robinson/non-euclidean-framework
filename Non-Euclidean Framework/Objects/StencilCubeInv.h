#pragma once
#ifndef STENCILCUBEINV_H
#define STENCILCUBEINV_H

#include "Face.h"

class StencilCubeInv
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