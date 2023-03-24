#ifndef FXAA_H
#define FXAA_H

#include "structures.h"
#include "ConstantBuffer.h"

class FXAA
{
public:
	bool Initialize( ID3D11Device* pDevice, ID3D11DeviceContext* pContext );
	void UpdateCB( int width, int height );

	inline void SetSpanMax( float spanMax ) noexcept { m_fSpanMax = spanMax; }
	inline void SetReduceMin( float reduceMin ) noexcept { m_fReduceMin = reduceMin; }
	inline void SetReduceMul( float reduceMul ) noexcept { m_fReduceMul = reduceMul; }

	inline float GetSpanMax() const noexcept { return m_fSpanMax; }
	inline float GetReduceMin() const noexcept { return m_fReduceMin; }
	inline float GetReduceMul() const noexcept { return m_fReduceMul; }

	inline bool IsActive() const noexcept { return m_bUseFXAA; }
	inline void SetUseFXAA( bool useFXAA ) noexcept { m_bUseFXAA = useFXAA; }
	inline ID3D11Buffer* const* GetCB() const noexcept { return m_cbFXAA.GetAddressOf(); }
private:
	FLOAT m_fSpanMax = 8.0f;
	FLOAT m_fReduceMin = 1.0f / 128.0f;
	FLOAT m_fReduceMul = 1.0f / 8.0f;
	BOOL m_bUseFXAA = FALSE;
	ConstantBuffer<FXAA_CB> m_cbFXAA;
};

#endif