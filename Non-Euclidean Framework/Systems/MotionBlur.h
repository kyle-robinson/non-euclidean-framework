#pragma once
#ifndef MOTIONBLUR_H
#define MOTIONBLUR_H

#include "structures.h"
#include "ConstantBuffer.h"

class MotionBlur
{
public:
	bool Initialize( ID3D11Device* pDevice, ID3D11DeviceContext* pContext );
	void UpdateCB();
	void SpawnControlWindow( bool usingFXAA );

	inline bool IsActive() const noexcept { return m_bUseMotionBlur; }
	inline ID3D11Buffer* const* GetCB() const noexcept { return m_cbMotionBlur.GetAddressOf(); }
	inline void SetViewProjInv( const XMMATRIX& matrix ) noexcept { XMStoreFloat4x4( &m_ViewProjInv, matrix ); }
	inline void SetPrevViewProj( const XMMATRIX& matrix ) noexcept { XMStoreFloat4x4( &m_PrevViewProj, matrix ); }
private:
	XMFLOAT4X4 m_ViewProjInv;
	XMFLOAT4X4 m_PrevViewProj;
	BOOL m_bUseMotionBlur = FALSE;
	int m_numSamples = 2;
	ConstantBuffer<MotionBlur_CB> m_cbMotionBlur;
};

#endif