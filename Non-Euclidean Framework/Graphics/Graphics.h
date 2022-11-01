#pragma once
#ifndef GRAPHICS_H
#define GRAPHICS_H

#define RENDER_DEPTH 3

#include "Quad.h"
#include "Shaders.h"
#include "Sampler.h"
#include "Viewport.h"
#include "SwapChain.h"
#include "Rasterizer.h"
#include "DepthStencil.h"
#include "RenderTarget.h"

class Graphics
{
public:
	bool Initialize( HWND hWnd, UINT width, UINT height );
	void UpdateRenderStateSkysphere();
	void UpdateRenderStateCube();
	void RenderSceneToTexture();
	void EndFrame();

	inline UINT GetWidth() const noexcept { return m_viewWidth; }
	inline UINT GetHeight() const noexcept { return m_viewHeight; }
	inline ID3D11Device* GetDevice() const noexcept { return m_pDevice.Get(); }
	inline ID3D11DeviceContext* GetContext() const noexcept { return m_pContext.Get(); }

	inline Bind::DepthStencil* GetDepthStencil() const noexcept { return &*m_pDepthStencil; }
	inline Bind::RenderTarget* GetRenderTarget() const noexcept { return &*m_pRenderTarget; }
	inline Bind::RenderTarget* GetBackBuffer() const noexcept { return &*m_pBackBuffer; }
	inline Bind::RenderTarget* GetCubeBuffer( uint32_t index ) const noexcept { return &*m_pCubeBuffers.at( index ); }
	inline float* GetClearColor() noexcept { return m_clearColor; }

private:
	void InitializeDirectX( HWND hWnd );
	bool InitializeShaders();
	bool InitializeRTT();

	// Window data
	Quad m_quad;
	UINT m_viewWidth;
	UINT m_viewHeight;
	float m_clearColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };

	// Shaders
	VertexShader m_vertexShader;
	PixelShader m_pixelShader;
	VertexShader m_vertexShaderPP;
	PixelShader m_pixelShaderPP;
	VertexShader m_vertexShaderOBJ;
	PixelShader m_pixelShaderOBJ;

	// Pipeline components
	std::shared_ptr<Bind::SwapChain> m_pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pContext;

	std::shared_ptr<Bind::Viewport> m_pViewport;
	std::shared_ptr<Bind::RenderTarget> m_pBackBuffer;
	std::shared_ptr<Bind::RenderTarget> m_pRenderTarget;
	std::shared_ptr<Bind::DepthStencil> m_pDepthStencil;
	std::unordered_map<uint32_t, std::shared_ptr<Bind::RenderTarget>> m_pCubeBuffers;
	std::unordered_map<Bind::Sampler::Type, std::shared_ptr<Bind::Sampler>> m_pSamplerStates;
	std::unordered_map<Bind::Rasterizer::Type, std::shared_ptr<Bind::Rasterizer>> m_pRasterizerStates;
};

#endif