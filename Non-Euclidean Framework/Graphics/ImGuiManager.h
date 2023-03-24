#pragma once
#ifndef IMGUIMANAGER_H
#define IMGUIMANAGER_H

class FXAA;
class MotionBlur;

/// <summary>
/// The main manager class for setting up ImGui components.
/// Contains a function to control the main graphical components of a scene from Graphics.h
/// </summary>
class ImGuiManager
{
public:
	ImGuiManager();
	~ImGuiManager();
	bool Initialize( HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* context ) const noexcept;
	void BeginRender() const noexcept;
	void EndRender() const noexcept;

	void SceneWindow( UINT width, UINT height, ID3D11ShaderResourceView* pTexture ) noexcept;
	void InstructionWindow() const noexcept;
	void PostProcessingWindow( FXAA* pFxaa, MotionBlur* pMotionBlur ) noexcept;
private:
	void SetBlackGoldStyle();
	void SetCustomStyle();
};

#endif