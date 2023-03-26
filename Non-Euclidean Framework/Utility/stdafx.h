#pragma once
#ifndef STDAFX_H
#define STDAFX_H

#include <memory>
#include <algorithm>

// Data Structures
#include <unordered_map>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <array>
#include <map>

// Windows API
#include <Windows.h>
#include <d3d11_1.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>

#define _XM_NO_INTRINSICS_
using namespace DirectX;

// Classes
#include "ErrorLogger.h"

// ImGui
#include <imgui/imgui.h>
#define SLIDER_HINT_TEXT "CTRL + click the slider to input a value directly."
#define DRAG_HINT_TEXT "Double click the drag slider to input a value directly."
#define COLOR_PICKER_HINT_TEXT "Click the color image to open a colour picker."

static void HelpMarker( const char* desc )
{
	ImGui::TextDisabled( "(?)" );
	if ( ImGui::IsItemHovered( ImGuiHoveredFlags_DelayShort ) )
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos( ImGui::GetFontSize() * 35.0f );
		ImGui::TextUnformatted( desc );
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

#endif