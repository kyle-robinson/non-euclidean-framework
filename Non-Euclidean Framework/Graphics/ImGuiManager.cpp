#include "stdafx.h"
#include "ImGuiManager.h"
#include "FXAA.h"
#include "Camera.h"
#include "MotionBlur.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include <imgui/imgui_internal.h>

ImGuiManager::ImGuiManager()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
    SetCustomStyle();
}

ImGuiManager::~ImGuiManager()
{
	ImGui::DestroyContext();
}

bool ImGuiManager::Initialize( HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext ) const noexcept
{
    if ( !ImGui_ImplWin32_Init( hWnd ) || !ImGui_ImplDX11_Init( pDevice, pContext ) )
        return false;
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    return true;
}

void ImGuiManager::BeginRender() const noexcept
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport( ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode );
    ImGui::ShowDemoWindow();
    ImGui::ShowMetricsWindow();
}

void ImGuiManager::EndRender() const noexcept
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData() );
}

void ImGuiManager::SceneWindow( UINT width, UINT height, ID3D11ShaderResourceView* pTexture ) noexcept
{
    ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0.0f, 0.0f ) );
    if ( ImGui::Begin( "Scene Window", FALSE ) )
    {
        // Add the scene texture to an imgui window and render
        ImVec2 vRegionMax = ImGui::GetWindowContentRegionMax();
        ImVec2 vImageMax = ImVec2(
            vRegionMax.x + ImGui::GetWindowPos().x,
            vRegionMax.y + ImGui::GetWindowPos().y );

        ImVec2 vRatio =
        {
            width / ImGui::GetWindowSize().x,
            height / ImGui::GetWindowSize().y
        };

        bool bIsFitToWidth = vRatio.x < vRatio.y ? true : false;
        ImVec2 ivMax =
        {
            bIsFitToWidth ? width / vRatio.y : vRegionMax.x,
            bIsFitToWidth ? vRegionMax.y : height / vRatio.x
        };

        ImVec2 pos = ImGui::GetCursorScreenPos();
        XMFLOAT2 half = { ( ivMax.x - vRegionMax.x ) / 2, ( ivMax.y - vRegionMax.y ) / 2 };
        ImVec2 vHalfPos = { pos.x - half.x, pos.y - half.y };

        ImVec2 ivMaxPos =
        {
            ivMax.x + ImGui::GetWindowPos().x - half.x,
            ivMax.y + ImGui::GetWindowPos().y - half.y
        };

        ImGui::GetWindowDrawList()->AddImage( (void*)pTexture, vHalfPos, ivMaxPos );
    }
    ImGui::End();
    ImGui::PopStyleVar();
}

void ImGuiManager::CameraWindow( Camera* pCamera ) const noexcept
{
	if ( ImGui::Begin( "Camera", FALSE, ImGuiWindowFlags_AlwaysAutoResize ) )
	{
        float movementSpeed = pCamera->GetSpeed();
        float nearPlane = pCamera->GetNearZ();
        float farPlane = pCamera->GetFarZ();
        float fov = pCamera->GetFovDegrees();

        ImGui::SetNextItemOpen( true, ImGuiCond_Once );
        if ( ImGui::TreeNode( "Instructions" ) )
        {
            ImGuiStyle& style = ImGui::GetStyle();
            if ( ImGui::BeginTable( "Controls", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_Resizable ) )
            {
                ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( style.ItemSpacing.x, style.CellPadding.y * 2 ) );
                ImGui::PopStyleVar();
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text( "W A S D" );
                ImGui::TableNextColumn();
                ImGui::Text( "Move Around" );

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text( "CTRL" );
                ImGui::TableNextColumn();
                ImGui::Text( "Move Down" );

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text( "SPACE" );
                ImGui::TableNextColumn();
                ImGui::Text( "Move Up" );

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text( "Right Mouse" );
                ImGui::TableNextColumn();
                ImGui::Text( "Look Around" );

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text( "F1" );
                ImGui::TableNextColumn();
                ImGui::Text( "Show ImGui Windows" );

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text( "F2" );
                ImGui::TableNextColumn();
                ImGui::Text( "Hide ImGui Windows" );

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text( "ESC" );
                ImGui::TableNextColumn();
                ImGui::Text( "Close Application" );

                ImGui::EndTable();
            }
            ImGui::TreePop();
        }

        ImGui::SetNextItemOpen( true, ImGuiCond_Once );
        if ( ImGui::TreeNode( "Controls" ) )
        {
            static bool useCollisions = pCamera->CanCollide();
            ImGui::Checkbox( "World Collisions?", &useCollisions );
            useCollisions ?
                pCamera->EnableCollisions() :
                pCamera->DisableCollisions();

            ImGui::Text( "Near Plane" );
            ImGui::SameLine();
            HelpMarker( DRAG_HINT_TEXT );
            if ( ImGui::DragFloat( "##Near Plane", &nearPlane, 0.01f, 0.01f, 10.0f ) )
                pCamera->SetNearZ( nearPlane );

            ImGui::Text( "Far Plane" );
            ImGui::SameLine();
            HelpMarker( DRAG_HINT_TEXT );
            if ( ImGui::DragFloat( "##Far Plane", &farPlane, 0.1f, 100.0f, 1000.0f ) )
                pCamera->SetFarZ( farPlane );

            ImGui::Text( "Field of View" );
            ImGui::SameLine();
            HelpMarker( SLIDER_HINT_TEXT );
            if ( ImGui::SliderFloat( "##Field of View", &fov, 50.0f, 110.0f, "%1.f" ) )
                pCamera->SetFovDegrees( fov );

            ImGui::Text( "Movement Speed" );
            ImGui::SameLine();
            HelpMarker( SLIDER_HINT_TEXT );
            if ( ImGui::SliderFloat( "##Movement Speed", &movementSpeed, 0.1f, 5.0f, "%.1f" ) )
                pCamera->SetSpeed( movementSpeed );

            ImGui::TreePop();
        }
	}
    ImGui::End();
}

void ImGuiManager::PostProcessingWindow( FXAA* pFxaa, MotionBlur* pMotionBlur ) noexcept
{
    if ( ImGui::Begin( "Post-Processing", FALSE, ImGuiWindowFlags_AlwaysAutoResize ) )
    {
        static bool useMotionBlur = pMotionBlur->IsActive();
        static bool useFXAA = pFxaa->IsActive();

        // Motion blur
        if ( pFxaa->IsActive() )
        {
            useMotionBlur = false;
            pMotionBlur->SetUseMotionBlur( false );
        }

        ImGui::Checkbox( "Use Motion Blur?", &useMotionBlur );
        pMotionBlur->SetUseMotionBlur( useMotionBlur );

        if ( useMotionBlur )
        {
            ImGui::Text( "No. Of Samples" );
            ImGui::SameLine();
            HelpMarker( SLIDER_HINT_TEXT );
            static int numSamples = pMotionBlur->GetNumSamples();
            if ( ImGui::SliderInt( "##No. Of Samples", &numSamples, 1, 10 ) )
                pMotionBlur->SetNumSamples( numSamples );
        }

        // FXAA
        if ( useMotionBlur )
        {
            useFXAA = false;
            pFxaa->SetUseFXAA( false );
        }

        ImGui::Checkbox( "Use FXAA?", &useFXAA );
        pFxaa->SetUseFXAA( useFXAA );

        if ( useFXAA )
        {
            ImGui::Text( "Max Span" );
            ImGui::SameLine();
            HelpMarker( SLIDER_HINT_TEXT );
            static float spanMax = pFxaa->GetSpanMax();
            if ( ImGui::SliderFloat( "##Max Span", &spanMax, 4.0f, 12.0f, "%1.f" ) )
                pFxaa->SetSpanMax( spanMax );

            ImGui::Text( "Min Reduce ( 1.0f / [value] )" );
            ImGui::SameLine();
            HelpMarker( SLIDER_HINT_TEXT );
            static float reduceMinDenom = 128.0f;
            if ( ImGui::SliderFloat( "##Min Reduce", &reduceMinDenom, 64.0f, 128.0f, "%1.f" ) )
                pFxaa->SetReduceMin( 1.0f / reduceMinDenom );

            ImGui::Text( "Mul Reduce ( 1.0f / [value] )" );
            ImGui::SameLine();
            HelpMarker( SLIDER_HINT_TEXT );
            static float reduceMulDenom = 8.0f;
            ImGui::SliderFloat( "##Mul Reduce", &reduceMulDenom, 4.0f, 12.0f, "%1.f" );
                pFxaa->SetReduceMul( 1.0f / reduceMulDenom );
        }
    }
    ImGui::End();
}

void ImGuiManager::SetCustomStyle()
{
    auto& style = ImGui::GetStyle();

    style.WindowPadding = ImVec2( 15, 15 );
    style.WindowRounding = 10.0f;
    style.FramePadding = ImVec2( 5, 5 );
    style.FrameRounding = 12.0f; // Make all elements (checkboxes, etc) circles
    style.ItemSpacing = ImVec2( 12, 8 );
    style.ItemInnerSpacing = ImVec2( 8, 6 );
    style.IndentSpacing = 25.0f;
    style.ScrollbarSize = 15.0f;
    style.ScrollbarRounding = 9.0f;
    style.GrabMinSize = 20.0f; // Make grab a circle
    style.GrabRounding = 12.0f;
    style.PopupRounding = 7.f;
    style.Alpha = 1.0;

    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text] = ImVec4( 0.00f, 0.00f, 0.00f, 1.00f );
    colors[ImGuiCol_TextDisabled] = ImVec4( 0.32f, 0.32f, 0.32f, 1.00f );

    colors[ImGuiCol_WindowBg] = ImVec4( 0.85f, 0.85f, 0.85f, 1.00f );
    colors[ImGuiCol_ChildBg] = ImVec4( 0.00f, 0.00f, 0.00f, 0.00f );
    colors[ImGuiCol_PopupBg] = ImVec4( 1.00f, 1.00f, 1.00f, 0.98f );
    colors[ImGuiCol_Border] = ImVec4( 0.00f, 0.00f, 0.00f, 0.30f );
    colors[ImGuiCol_BorderShadow] = ImVec4( 1.00f, 1.00f, 1.00f, 0.00f );

    colors[ImGuiCol_FrameBg] = ImVec4( 1.00f, 1.00f, 1.00f, 0.60f );
    colors[ImGuiCol_FrameBgHovered] = ImVec4( 0.85f, 0.85f, 0.85f, 0.30f );
    colors[ImGuiCol_FrameBgActive] = ImVec4( 0.66f, 0.66f, 0.66f, 0.67f );

    colors[ImGuiCol_TitleBg] = ImVec4( 0.96f, 0.96f, 0.96f, 1.00f );
    colors[ImGuiCol_TitleBgActive] = ImVec4( 0.82f, 0.82f, 0.82f, 1.00f );
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4( 1.00f, 1.00f, 1.00f, 0.51f );
    colors[ImGuiCol_MenuBarBg] = ImVec4( 0.86f, 0.86f, 0.86f, 1.00f );

    colors[ImGuiCol_ScrollbarBg] = ImVec4( 0.98f, 0.98f, 0.98f, 0.53f );
    colors[ImGuiCol_ScrollbarGrab] = ImVec4( 0.69f, 0.69f, 0.69f, 0.80f );
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4( 0.49f, 0.49f, 0.49f, 0.80f );
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4( 0.49f, 0.49f, 0.49f, 1.00f );

    colors[ImGuiCol_CheckMark] = ImVec4( 0.00f, 0.00f, 0.00f, 1.00f );
    colors[ImGuiCol_SliderGrab] = ImVec4( 1.000f, 0.777f, 0.578f, 0.780f );
    colors[ImGuiCol_SliderGrabActive] = ImVec4( 1.000f, 0.987f, 0.611f, 0.600f );

    colors[ImGuiCol_Button] = ImVec4( 1.00f, 0.77f, 0.00f, 1.00f );
    colors[ImGuiCol_ButtonHovered] = ImVec4( 1.00f, 1.00f, 0.00f, 1.00f );
    colors[ImGuiCol_ButtonActive] = ImVec4( 0.84f, 0.97f, 0.01f, 1.00f );

    colors[ImGuiCol_Header] = ImVec4( 1.00f, 1.00f, 1.00f, 0.60f );
    colors[ImGuiCol_HeaderHovered] = ImVec4( 0.85f, 0.85f, 0.85f, 0.30f );
    colors[ImGuiCol_HeaderActive] = ImVec4( 0.66f, 0.66f, 0.66f, 0.67f );

    colors[ImGuiCol_Separator] = ImVec4( 0.39f, 0.39f, 0.39f, 1.00f );
    colors[ImGuiCol_SeparatorHovered] = ImVec4( 1.00f, 1.00f, 1.00f, 0.78f );
    colors[ImGuiCol_SeparatorActive] = ImVec4( 1.00f, 1.00f, 1.00f, 1.00f );
    colors[ImGuiCol_ResizeGrip] = ImVec4( 0.80f, 0.80f, 0.80f, 0.56f );
    colors[ImGuiCol_ResizeGripHovered] = ImVec4( 1.00f, 1.00f, 1.00f, 0.67f );
    colors[ImGuiCol_ResizeGripActive] = ImVec4( 1.00f, 1.00f, 1.00f, 0.95f );

    colors[ImGuiCol_Tab] = ImVec4( 1.00f, 0.54f, 0.01f, 0.71f );
    colors[ImGuiCol_TabHovered] = ImVec4( 0.96f, 0.73f, 0.09f, 0.90f );
    colors[ImGuiCol_TabActive] = ImVec4( 1.00f, 0.97f, 0.00f, 1.00f );
    colors[ImGuiCol_TabUnfocused] = ImVec4( 0.92f, 0.93f, 0.94f, 0.99f );
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4( 1.00f, 1.00f, 1.00f, 1.00f );

    colors[ImGuiCol_PlotLines] = ImVec4( 0.39f, 0.39f, 0.39f, 1.00f );
    colors[ImGuiCol_PlotLinesHovered] = ImVec4( 1.00f, 0.43f, 0.35f, 1.00f );
    colors[ImGuiCol_PlotHistogram] = ImVec4( 0.90f, 0.70f, 0.00f, 1.00f );
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4( 1.00f, 0.45f, 0.00f, 1.00f );

    colors[ImGuiCol_TextSelectedBg] = ImVec4( 1.00f, 1.00f, 1.00f, 0.35f );
    colors[ImGuiCol_DragDropTarget] = ImVec4( 1.00f, 1.00f, 1.00f, 0.95f );
    colors[ImGuiCol_NavHighlight] = ImVec4( 1.00f, 1.00f, 1.00f, 0.80f );
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4( 0.70f, 0.70f, 0.70f, 0.70f );
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4( 0.20f, 0.20f, 0.20f, 0.20f );
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4( 0.20f, 0.20f, 0.20f, 0.35f );
}