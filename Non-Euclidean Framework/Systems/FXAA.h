#ifndef FXAA_H
#define FXAA_H

#include "structures.h"
#include "ConstantBuffer.h"

class FXAA
{
public:
	bool Initialize( ID3D11Device* pDevice, ID3D11DeviceContext* pContext );
	void UpdateCB( int width, int height );
	void SpawnControlWindow( bool usingMotionBlur );

	inline bool IsActive() const noexcept { return m_bUseFXAA; }
	inline ID3D11Buffer* const* GetCB() const noexcept { return m_cbFXAA.GetAddressOf(); }
private:
	FLOAT m_fSpanMax = 8.0f;
	FLOAT m_fReduceMin = 1.0f / 128.0f;
	FLOAT m_fReduceMul = 1.0f / 8.0f;
	BOOL m_bUseFXAA = FALSE;
	ConstantBuffer<FXAA_CB> m_cbFXAA;
};

#endif