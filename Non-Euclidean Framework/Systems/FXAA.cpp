#include "stdafx.h"
#include "FXAA.h"
#include <imgui/imgui.h>

bool FXAA::Initialize( ID3D11Device* pDevice, ID3D11DeviceContext* pContext )
{
	try
	{
		HRESULT hr = m_cbFXAA.Initialize( pDevice, pContext );
		COM_ERROR_IF_FAILED( hr, "Failed to create 'FXAA' constant buffer!" );
	}
	catch ( COMException& exception )
	{
		ErrorLogger::Log( exception );
		return false;
	}
	return true;
}

void FXAA::UpdateCB( int width, int height )
{
	// Setup mapping data
	FXAAData fxaaData;
	fxaaData.TextureSizeInverse = XMFLOAT2( 1.0f / width, 1.0f / height );
	fxaaData.SpanMax = m_fSpanMax;
	fxaaData.ReduceMin = m_fReduceMin;
	fxaaData.ReduceMul = m_fReduceMul;
	fxaaData.UseFXAA = m_bUseFXAA;

	// Add to constant buffer
	m_cbFXAA.data.FXAA = fxaaData;
    if ( !m_cbFXAA.ApplyChanges() ) return;
}

void FXAA::SpawnControlWindow( bool usingMotionBlur )
{
	ImGui::SameLine();

	static bool useFXAA = m_bUseFXAA;
	if ( usingMotionBlur )
	{
		useFXAA = false;
		m_bUseFXAA = useFXAA;
		return;
	}

	ImGui::Checkbox( "Use FXAA?", &useFXAA );
	m_bUseFXAA = useFXAA;

	if ( m_bUseFXAA )
	{
		ImGui::Text( "Max Span" );
		ImGui::SliderFloat( "##Max Span", &m_fSpanMax, 4.0f, 12.0f, "%1.f" );

		ImGui::Text( "Min Reduce ( 1.0f / [value] )" );
		static float reduceMinDenom = 128.0f;
		ImGui::SliderFloat( "##Min Reduce", &reduceMinDenom, 64.0f, 128.0f, "%1.f" );
		m_fReduceMin = 1.0f / reduceMinDenom;

		ImGui::Text( "Mul Reduce ( 1.0f / [value] )" );
		static float reduceMulDenom = 8.0f;
		ImGui::SliderFloat( "##Mul Reduce", &reduceMulDenom, 4.0f, 12.0f, "%1.f" );
		m_fReduceMul = 1.0f / reduceMulDenom;
	}
}