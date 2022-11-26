#include "stdafx.h"
#include "NonEuclidean.h"
#include <imgui/imgui.h>

bool NonEuclidean::Initialize( ID3D11Device* pDevice, ID3D11DeviceContext* pContext )
{
	try
	{
		HRESULT hr = m_cbNonEuclidean.Initialize( pDevice, pContext );
		COM_ERROR_IF_FAILED( hr, "Failed to create 'Non-Euclidean' constant buffer!" );
	}
	catch ( COMException& exception )
	{
		ErrorLogger::Log( exception );
		return false;
	}
	return true;
}

void NonEuclidean::UpdateCB()
{
	// Setup mapping data
	NonEuclideanData ncData;
	ncData.CurveScale = m_fCurveScale;
	ncData.UseHyperbolic = m_bUseHyperbolic;
	ncData.UseElliptic = m_bUseElliptic;

	// Add to constant buffer
	m_cbNonEuclidean.data.NonEuclidean = ncData;
    if ( !m_cbNonEuclidean.ApplyChanges() ) return;
}

void NonEuclidean::SpawnControlWindow()
{
	if ( ImGui::Begin( "Non-Euclidean Space", FALSE, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove ) )
	{
		static bool useHyperbolic = m_bUseHyperbolic;
		static bool useElliptic = m_bUseElliptic;

        if ( ImGui::Checkbox( "Hyperbolic?", &useHyperbolic ) )
			useElliptic = FALSE;
		ImGui::SameLine();
        if ( ImGui::Checkbox( "Elliptic?", &useElliptic ) )
			useHyperbolic = FALSE;

		m_bUseHyperbolic = useHyperbolic;
		m_bUseElliptic = useElliptic;

		if ( m_bUseHyperbolic || m_bUseElliptic )
		{
			ImGui::Text( "Curve Scale" );
			ImGui::SliderFloat( "##Curve Scale", &m_fCurveScale, 1.0f, 2.0f, "%.1f" );
		}
	}
	ImGui::End();
}