#ifndef NONEUCLIDEAN_H
#define NONEUCLIDEAN_H

#include "structures.h"
#include "ConstantBuffer.h"

class NonEuclidean
{
public:
	bool Initialize( ID3D11Device* pDevice, ID3D11DeviceContext* pContext );
	void UpdateCB();
	void SpawnControlWindow();

	inline bool IsActive() const noexcept { return ( m_bUseHyperbolic || m_bUseElliptic ); }
	inline ID3D11Buffer* const* GetCB() const noexcept { return m_cbNonEuclidean.GetAddressOf(); }
private:
	FLOAT m_fCurveScale = 1.33f;
	BOOL m_bUseHyperbolic = FALSE;
	BOOL m_bUseElliptic = FALSE;
	ConstantBuffer<NonEuclidean_CB> m_cbNonEuclidean;
};

#endif