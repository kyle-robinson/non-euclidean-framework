#include "stdafx.h"
#include "DepthStencil.h"
#include "RenderTarget.h"
#include "PostProcessing.h"
#include <imgui/imgui.h>

void PostProcessing::Initialize( ID3D11Device* pDevice )
{
	postProcessBasic = std::make_shared<BasicPostProcess>( pDevice );
	postProcessToneMap = std::make_shared<ToneMapPostProcess>( pDevice );

	bloomBlurHorizontal = true;
	bloomBlurBrightness = 1.0f;
	bloomBlurSize = 1.0f;
	bloomThreshold = 1.0f;
	gaussianMultiplier = 1.0f;

	useBasicPostProcess = true;
	basicEffect = BasicPostProcess::Effect::Copy;
	toneMapOperator = ToneMapPostProcess::Operator::None;
	toneMapTransferFunction = ToneMapPostProcess::TransferFunction::Linear;
}

void PostProcessing::Bind( ID3D11DeviceContext* pContext, Bind::RenderTarget* pRenderTarget ) noexcept
{
	// Basic Post-Processing
	if ( useBasicPostProcess )
	{
		// Set main post-processing effect
		postProcessBasic->SetEffect( basicEffect );

		// Update bloom extract threshold
		if ( basicEffect == BasicPostProcess::Effect::BloomExtract )
			postProcessBasic->SetBloomExtractParameter( bloomThreshold );

		// Update gaussian blur effect multiplier
		if ( basicEffect == BasicPostProcess::Effect::GaussianBlur_5x5 )
			postProcessBasic->SetGaussianParameter( gaussianMultiplier );

		// Render post-processing effect to scene texture
		postProcessBasic->SetSourceTexture( pRenderTarget->GetShaderResourceView() );
		postProcessBasic->Process( pContext );

		// Set and render additional effect if using bloom effect
		if ( basicEffect == BasicPostProcess::Effect::BloomExtract )
		{
			postProcessBasic->SetBloomBlurParameters( bloomBlurHorizontal, bloomBlurSize, bloomBlurBrightness );
			postProcessBasic->SetEffect( BasicPostProcess::BloomBlur );
			postProcessBasic->Process( pContext );
		}
	}
	// Tone Map Post-Processing
	else
	{
		postProcessToneMap->SetOperator( toneMapOperator );
		postProcessToneMap->SetTransferFunction( toneMapTransferFunction );
		postProcessToneMap->SetHDRSourceTexture( pRenderTarget->GetShaderResourceView() );
		postProcessToneMap->Process( pContext );
	}
}

void PostProcessing::SpawnControlWindow()
{
	if ( ImGui::Begin( "Post-Processing", FALSE, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove ) )
	{
		// Switch between basic/tone map post-process options
		ImGui::Text( "Post-Process Type:" );
		ImGui::SameLine();
		static int postProcessGroup = 0;
		if ( ImGui::RadioButton( "Basic", &postProcessGroup, 0 ) )
			useBasicPostProcess = true;
		ImGui::SameLine();
		if ( ImGui::RadioButton( "Tone Map", &postProcessGroup, 1 ) )
			useBasicPostProcess = false;

		if ( useBasicPostProcess )
		{
			// Overlay effects
			static int activeEffect = 0;
			static bool selectedEffect[6];
			static std::string previewValueEffect = "None";
			static const char* effectList[]{ "None", "Sepia", "Monochrome", "Gaussian Blur", "Down-Scale", "Bloom" };
			if ( ImGui::BeginCombo( "Overlay Effect", previewValueEffect.c_str() ) )
			{
				for ( uint32_t i = 0; i < IM_ARRAYSIZE( effectList ); i++ )
				{
					const bool isSelected = i == activeEffect;
					if ( ImGui::Selectable( effectList[i], isSelected ) )
					{
						activeEffect = i;
						previewValueEffect = effectList[i];
					}
				}

				switch ( activeEffect )
				{
				case 0: basicEffect = BasicPostProcess::Effect::Copy; break;
				case 1: basicEffect = BasicPostProcess::Effect::Sepia; break;
				case 2: basicEffect = BasicPostProcess::Effect::Monochrome; break;
				case 3: basicEffect = BasicPostProcess::Effect::GaussianBlur_5x5; break;
				case 4: basicEffect = BasicPostProcess::Effect::DownScale_4x4; break;
				case 5: basicEffect = BasicPostProcess::Effect::BloomExtract; break;
				}

				ImGui::EndCombo();
			}

			// Allow modification of additional parameters when bloom blur is enabled
			if ( basicEffect == BasicPostProcess::Effect::BloomExtract )
			{
				ImGui::Text( "Blur Pass Type:" );
				ImGui::SameLine();
				static int bloomBlurGroup = 0;
				if ( ImGui::RadioButton( "Horizontal", &bloomBlurGroup, 0 ) )
					bloomBlurHorizontal = true;
				ImGui::SameLine();
				if ( ImGui::RadioButton( "Vertical", &bloomBlurGroup, 1 ) )
					bloomBlurHorizontal = false;

				ImGui::SliderFloat( "Bloom Threshold", &bloomThreshold, 0.0f, 5.0f, "%.1f" );
				ImGui::SliderFloat( "Bloom Blur Size", &bloomBlurSize, 0.0f, 5.0f, "%.1f" );
				ImGui::SliderFloat( "Bloom Blur Brightness", &bloomBlurBrightness, 0.0f, 5.0f, "%.1f" );
			}

			// Allow modification of additional parameters when gaussian blur is enabled
			if ( basicEffect == BasicPostProcess::Effect::GaussianBlur_5x5 )
				ImGui::SliderFloat( "Gaussian Multiplier", &gaussianMultiplier, 0.0f, 5.0f, "%.1f" );
		}
		else
		{
			// Tone map operator
			static int activeOperator = 0;
			static bool selectedOperator[4];
			static std::string previewValueOperator = "None";
			static const char* operatorList[]{ "None", "Saturate", "Reinhard", "ACESFilmic" };
			if ( ImGui::BeginCombo( "Operator", previewValueOperator.c_str() ) )
			{
				for ( uint32_t i = 0; i < IM_ARRAYSIZE( operatorList ); i++ )
				{
					const bool isSelected = i == activeOperator;
					if ( ImGui::Selectable( operatorList[i], isSelected ) )
					{
						activeOperator = i;
						previewValueOperator = operatorList[i];
					}
				}

				switch ( activeOperator )
				{
				case 0: toneMapOperator = ToneMapPostProcess::Operator::None; break;
				case 1: toneMapOperator = ToneMapPostProcess::Operator::Saturate; break;
				case 2: toneMapOperator = ToneMapPostProcess::Operator::Reinhard; break;
				case 3: toneMapOperator = ToneMapPostProcess::Operator::ACESFilmic; break;
				}

				ImGui::EndCombo();
			}

			// Tone map transfer function
			static int activeTransfer = 0;
			static bool selectedTransfer[3];
			static std::string previewValueTransfer = "Linear";
			static const char* transferList[]{ "Linear", "SRGB", "ST2084" };
			if ( ImGui::BeginCombo( "Transfer Function", previewValueTransfer.c_str() ) )
			{
				for ( uint32_t i = 0; i < IM_ARRAYSIZE( transferList ); i++ )
				{
					const bool isSelected = i == activeTransfer;
					if ( ImGui::Selectable( transferList[i], isSelected ) )
					{
						activeTransfer = i;
						previewValueTransfer = transferList[i];
					}
				}

				switch ( activeTransfer )
				{
				case 0: toneMapTransferFunction = ToneMapPostProcess::TransferFunction::Linear; break;
				case 1: toneMapTransferFunction = ToneMapPostProcess::TransferFunction::SRGB; break;
				case 2: toneMapTransferFunction = ToneMapPostProcess::TransferFunction::ST2084; break;
				}

				ImGui::EndCombo();
			}
		}
	}
	ImGui::End();
}