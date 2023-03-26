#include "stdafx.h"
#include "MotionBlur.h"

bool MotionBlur::Initialize( ID3D11Device* pDevice, ID3D11DeviceContext* pContext )
{
	try
	{
		HRESULT hr = m_cbMotionBlur.Initialize( pDevice, pContext );
		COM_ERROR_IF_FAILED( hr, "Failed to create 'MotionBlur' constant buffer!" );
	}
	catch ( COMException& exception )
	{
		ErrorLogger::Log( exception );
		return false;
	}
	return true;
}

void MotionBlur::UpdateCB()
{
	// Setup mapping data
	MotionBlurData mbData;
	mbData.mViewProjectionInverse = XMLoadFloat4x4( &m_ViewProjInv );
	mbData.mPreviousViewProjection = XMLoadFloat4x4( &m_PrevViewProj );
	mbData.UseMotionBlur = m_bUseMotionBlur;
	mbData.NumSamples = m_numSamples;

	// Add to constant buffer
	m_cbMotionBlur.data.MotionBlur = mbData;
    if ( !m_cbMotionBlur.ApplyChanges() ) return;
}