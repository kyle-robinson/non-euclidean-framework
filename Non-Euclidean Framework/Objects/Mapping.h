#pragma once
#ifndef MAPPING_H
#define MAPPING_H

#include "structures.h"
#include "ConstantBuffer.h"

class Mapping
{
public:
	bool Initialize( ID3D11Device* pDevice, ID3D11DeviceContext* pContext );
	void UpdateCB();
	void SpawnControlWindow();
	inline ID3D11Buffer* const* GetMappingCB() const noexcept { return m_cbMapping.GetAddressOf(); }
private:
	BOOL m_bUseNormalMap = TRUE;
	BOOL m_bUseParallaxMap = TRUE;
	BOOL m_bUseParallaxOcclusion = TRUE;
	BOOL m_bUseParallaxSelfShadowing = TRUE;
	BOOL m_bUseSoftShadow = TRUE;
	FLOAT m_fHeightScale = 0.1f;
	ConstantBuffer<Mapping_CB> m_cbMapping;
};

#endif