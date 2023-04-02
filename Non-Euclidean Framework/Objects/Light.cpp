#include "stdafx.h"
#include "Light.h"
#include "Camera.h"

bool Light::Initialize( ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ConstantBuffer<Matrices>& cb_vs_vertexshader )
{
	try
	{
#if _x64
		// Initialize light model
        if ( !m_objLight.Initialize( "Resources\\Models\\light.obj", pDevice, pContext, cb_vs_vertexshader ) )
		    return false;
        m_objLight.SetInitialScale( 0.1f, 0.1f, 0.1f );
#endif

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
#if _x64
    if ( !m_bAttachedToCamera )
        m_objLight.Draw( view, projection );
#endif
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
    light.SpotAngle = XMConvertToRadians( m_fSpotAngle );
    light.ConstantAttenuation = m_fConstantAttenuation;
    light.LinearAttenuation = m_fLinearAttenuation;
    light.QuadraticAttenuation = m_fQuadraticAttenuation;
    light.Intensity = m_fIntensity;

    // Setup light
    if ( m_bAttachedToCamera )
        m_fPosition = cameraPosition;
    light.Position = m_fPosition;

#if _x64
    // Update model position
    m_objLight.SetPosition( XMFLOAT3( m_fPosition.x, m_fPosition.y, m_fPosition.z ) );
#endif

    XMVECTOR lightDirection = XMVectorSet(
        camera.GetCameraTarget().x - m_fPosition.x,
        camera.GetCameraTarget().y - m_fPosition.y,
        camera.GetCameraTarget().z - m_fPosition.z,
        0.0f
    );
    lightDirection = XMVector3Normalize( lightDirection );
    XMStoreFloat4( &light.Direction, lightDirection );

    // Add to constant buffer
    m_cbLight.data.EyePosition = cameraPosition;
    m_cbLight.data.Lights[0] = light;
    if ( !m_cbLight.ApplyChanges() ) return;
}

void Light::SpawnControlWindow()
{
    if ( ImGui::Begin( "Light", FALSE, ImGuiWindowFlags_AlwaysAutoResize ) )
    {
        ImGui::Checkbox( "Attached To Camera?", &m_bAttachedToCamera );

        if ( !m_bAttachedToCamera )
        {
            ImGui::Text( "Position" );
            ImGui::SameLine();
            HelpMarker( DRAG_HINT_TEXT );
		    ImGui::DragFloat4( "##Position", &m_fPosition.x, 0.1f, -10.0f, 10.0f, "%.1f" );
            ImGui::Separator();
        }

        ImGui::Text( "Color" );
        ImGui::SameLine();
        HelpMarker( COLOR_PICKER_HINT_TEXT );
		ImGui::ColorEdit4( "##Color", &m_fColor.x, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB );

        ImGui::Text( "Constant Attenuation" );
        ImGui::SameLine();
        HelpMarker( DRAG_HINT_TEXT );
		ImGui::DragFloat( "##Constant", &m_fConstantAttenuation, 0.001f, 0.0f, 1.0f );

        ImGui::Text( "Linear Attenuation" );
        ImGui::SameLine();
        HelpMarker( DRAG_HINT_TEXT );
		ImGui::DragFloat( "##Linear", &m_fLinearAttenuation, 0.001f, 0.0f, 1.0f );

        ImGui::Text( "Quadratic Attenuation" );
        ImGui::SameLine();
        HelpMarker( DRAG_HINT_TEXT );
		ImGui::DragFloat( "##Quadratic", &m_fQuadraticAttenuation, 0.01f, 0.0f, 1.0f );

        ImGui::Text( "Intensity" );
        ImGui::SameLine();
        HelpMarker( DRAG_HINT_TEXT );
		ImGui::DragFloat( "##Intensity", &m_fIntensity, 0.1f, 1.0f, 10.0f );
    }
    ImGui::End();
}