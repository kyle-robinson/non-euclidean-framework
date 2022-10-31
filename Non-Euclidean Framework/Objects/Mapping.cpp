#include "stdafx.h"
#include "Mapping.h"
#include <imgui/imgui.h>

bool Mapping::Initialize( ID3D11Device* pDevice, ID3D11DeviceContext* pContext )
{
	try
	{
		HRESULT hr = m_cbMapping.Initialize( pDevice, pContext );
		COM_ERROR_IF_FAILED( hr, "Failed to create 'Mapping' constant buffer!" );
	}
	catch ( COMException& exception )
	{
		ErrorLogger::Log( exception );
		return false;
	}
	return true;
}

void Mapping::UpdateCB()
{
	// Setup mapping data
	MappingData mapData;
	mapData.UseNormalMap = m_bUseNormalMap;
	mapData.UseParallaxMap = m_bUseParallaxMap;
	mapData.UseParallaxOcclusion = m_bUseParallaxOcclusion;
	mapData.UseParallaxSelfShadowing = m_bUseParallaxSelfShadowing;
	mapData.UseSoftShadow = m_bUseSoftShadow;
	mapData.HeightScale = m_fHeightScale;

	// Add to constant buffer
	m_cbMapping.data.MapData = mapData;
    if ( !m_cbMapping.ApplyChanges() ) return;
}

void Mapping::SpawnControlWindow()
{
	if ( ImGui::Begin( "Texture Mapping", FALSE, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove ) )
	{
		// normal & parallax mapping
		static int activeMappingTechnique = 2;
		static bool selectedMappingTechnique[3];
		static std::string previewValueMappingTechnique = "Normal + Parallax Mapping";
		static const char* mappingList[]{ "None", "Normal Mapping", "Normal + Parallax Mapping" };
		ImGui::Text( "Mapping Technique" );
		if ( ImGui::BeginCombo( "##Mapping Technique", previewValueMappingTechnique.c_str() ) )
		{
			for ( uint32_t i = 0; i < IM_ARRAYSIZE( mappingList ); i++ )
			{
				const bool isSelected = i == activeMappingTechnique;
				if ( ImGui::Selectable( mappingList[i], isSelected ) )
				{
					activeMappingTechnique = i;
					previewValueMappingTechnique = mappingList[i];
				}
			}

			switch ( activeMappingTechnique )
			{
			case 0: m_bUseNormalMap = false; m_bUseParallaxMap = false; m_bUseParallaxOcclusion = false; m_bUseParallaxSelfShadowing = false; break;
			case 1: m_bUseNormalMap = true; m_bUseParallaxMap = false; m_bUseParallaxOcclusion = false; m_bUseParallaxSelfShadowing = false; break;
			case 2: m_bUseNormalMap = true; m_bUseParallaxMap = true; m_bUseParallaxOcclusion = false; m_bUseParallaxSelfShadowing = false; break;
			}

			ImGui::EndCombo();
		}

		// parallax occlusion and self-shadowing
		if ( m_bUseParallaxMap )
		{
			// parallax depth scale
			ImGui::NewLine();
			ImGui::Text( "Height Scale" );
			ImGui::SliderFloat( "##Height Scale", &m_fHeightScale, 0.0f, 1.0f, "%.1f" );

			ImGui::NewLine();
			ImGui::Separator();
			ImGui::NewLine();

			static int activeParallaxTechnique = 2;
			static bool selectedParallaxTechnique[3];
			static std::string previewValueParallaxTechnique = "Occlusion + Self-Shadowing";
			static const char* parallaxList[]{ "Basic", "Occlusion Mapping", "Occlusion + Self-Shadowing" };
			ImGui::Text( "Parallax Technique" );
			if ( ImGui::BeginCombo( "##Parallax Technique", previewValueParallaxTechnique.c_str() ) )
			{
				for ( uint32_t i = 0; i < IM_ARRAYSIZE( parallaxList ); i++ )
				{
					const bool isSelected = i == activeParallaxTechnique;
					if ( ImGui::Selectable( parallaxList[i], isSelected ) )
					{
						activeParallaxTechnique = i;
						previewValueParallaxTechnique = parallaxList[i];
					}
				}

				switch ( activeParallaxTechnique )
				{
				case 0: m_bUseParallaxOcclusion = false; m_bUseParallaxSelfShadowing = false; break;
				case 1: m_bUseParallaxOcclusion = true; m_bUseParallaxSelfShadowing = false; break;
				case 2: m_bUseParallaxOcclusion = true; m_bUseParallaxSelfShadowing = true; break;
				}

				ImGui::EndCombo();
			}

			// soft and hard shadows
			if ( m_bUseParallaxSelfShadowing )
			{
				ImGui::NewLine();
				ImGui::Separator();
				ImGui::NewLine();

				static int shadowGroup = 0;
				if ( ImGui::RadioButton( "Soft Shadows", &shadowGroup, 0 ) )
					m_bUseSoftShadow = true;
				ImGui::SameLine();
				if ( ImGui::RadioButton( "Hard Shadows", &shadowGroup, 1 ) )
					m_bUseSoftShadow = false;
			}
		}
	}
	ImGui::End();
}