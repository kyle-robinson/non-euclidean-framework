#include "stdafx.h"
#include "Light.h"
#include "Camera.h"
#include <imgui/imgui.h>

bool Light::Initialize( ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ConstantBuffer<Matrices>& cb_vs_vertexshader )
{
	try
	{
		// Initialize light model
        if ( !m_objLight.Initialize( "Resources\\Models\\light.obj", pDevice, pContext, cb_vs_vertexshader ) )
		    return false;
        m_objLight.SetInitialScale( 0.1f, 0.1f, 0.1f );

        // Initialize constant buffer
		HRESULT hr = m_cbLight.Initialize( pDevice, pContext );
		COM_ERROR_IF_FAILED( hr, "Failed to create 'Light' constant buffer!" );
	}
	catch ( COMException& exception )
	{
		ErrorLogger::Log( exception );
		return false;
	}
	return true;
}

void Light::Draw( const XMMATRIX& view, const XMMATRIX& projection )
{
    if ( !m_bAttachedToCamera )
        m_objLight.Draw( view, projection );
}

void Light::UpdateCB( Camera& camera )
{
    XMFLOAT4 cameraPosition =
    {
        camera.GetPositionFloat3().x,
        camera.GetPositionFloat3().y,
        camera.GetPositionFloat3().z,
        1.0f
    };

	// Setup light data
    LightData light;
    light.Enabled = TRUE;
    light.LightType = PointLight;
    light.Color = m_fColor;
    light.SpotAngle = DirectX::XMConvertToRadians( m_fSpotAngle );
    light.ConstantAttenuation = m_fConstantAttenuation;
    light.LinearAttenuation = m_fLinearAttenuation;
    light.QuadraticAttenuation = m_fQuadraticAttenuation;
    light.Intensity = m_fIntensity;

    // Setup light
    if ( m_bAttachedToCamera )
        m_fPosition = cameraPosition;
    light.Position = m_fPosition;

    // Update model position
    m_objLight.SetPosition( XMFLOAT3( m_fPosition.x, m_fPosition.y, m_fPosition.z ) );

    DirectX::XMVECTOR lightDirection = DirectX::XMVectorSet(
        camera.GetCameraTarget().x - m_fPosition.x,
        camera.GetCameraTarget().y - m_fPosition.y,
        camera.GetCameraTarget().z - m_fPosition.z,
        0.0f
    );
    lightDirection = DirectX::XMVector3Normalize( lightDirection );
    DirectX::XMStoreFloat4( &light.Direction, lightDirection );

    // Add to constant buffer
    m_cbLight.data.EyePosition = cameraPosition;
    m_cbLight.data.Lights[0] = light;
    if ( !m_cbLight.ApplyChanges() ) return;
}

void Light::SpawnControlWindow()
{
    if ( ImGui::Begin( "Light Data", FALSE, ImGuiWindowFlags_AlwaysAutoResize ) )
    {
        ImGui::Checkbox( "Attached To Camera?", &m_bAttachedToCamera );
        ImGui::NewLine();

        if ( !m_bAttachedToCamera )
        {
            ImGui::Text( "Position" );
		    ImGui::SliderFloat4( "##Position", &m_fPosition.x, -10.0f, 10.0f, "%.1f" );
		    ImGui::NewLine();
            ImGui::Separator();
            ImGui::NewLine();
        }

        ImGui::Text( "Color" );
		ImGui::SliderFloat4( "##Color", &m_fColor.x, 0.0f, 1.0f, "%.1f" );
		ImGui::NewLine();

        ImGui::Text( "Constant Attenuation" );
		ImGui::SliderFloat( "##Constant", &m_fConstantAttenuation, 0.0f, 1.0f, "%.1f" );
		ImGui::NewLine();

        ImGui::Text( "Linear Attenuation" );
		ImGui::SliderFloat( "##Linear", &m_fLinearAttenuation, 0.0f, 1.0f, "%.1f" );
		ImGui::NewLine();

        ImGui::Text( "Quadratic Attenuation" );
		ImGui::SliderFloat( "##Quadratic", &m_fQuadraticAttenuation, 0.0f, 1.0f, "%.1f" );
		ImGui::NewLine();

        ImGui::Text( "Intensity" );
		ImGui::SliderFloat( "##Intensity", &m_fIntensity, 1.0f, 10.0f, "%1.f" );
    }
    ImGui::End();
}