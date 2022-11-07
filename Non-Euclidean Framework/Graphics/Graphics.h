#pragma once
#ifndef GRAPHICS_H
#define GRAPHICS_H

enum class Side;
static uint32_t RENDER_DEPTH = 5u;
static uint32_t CAMERA_COUNT = 6u;

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
	void BeginFrame();
	void BeginFrameCube( Side side, uint32_t index );

	void UpdateRenderStateSkysphere();
	void UpdateRenderStateCube();
	void UpdateRenderStateObject();
	void UpdateRenderStateTexture();
	
	void BeginRenderSceneToTexture();
	void RenderSceneToTexture( ID3D11Buffer* const* cbMotionBlur, ID3D11Buffer* const* cbFXAA );
	void EndFrame();

	inline UINT GetWidth() const noexcept { return m_viewWidth; }
	inline UINT GetHeight() const noexcept { return m_viewHeight; }
	inline ID3D11Device* GetDevice() const noexcept { return m_pDevice.Get(); }
	inline ID3D11DeviceContext* GetContext() const noexcept { return m_pContext.Get(); }

	inline Bind::RenderTarget* GetRenderTarget() const noexcept { return &*m_pRenderTarget; }
	inline Bind::RenderTarget* GetCubeBuffer( Side side, uint32_t index ) { return &*m_pCubeBuffers.at( side ).at( index ); }
	inline std::vector<std::shared_ptr<Bind::RenderTarget>> GetCubeBufferSide( Side side ) const noexcept { return m_pCubeBuffers.at( side ); }
	inline std::unordered_map<Side, std::vector<std::shared_ptr<Bind::RenderTarget>>> GetCubeBufferAll( Side side ) const noexcept { return m_pCubeBuffers; }

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
	VertexShader m_vertexShaderTEX;
	PixelShader m_pixelShaderTEX;
	VertexShader m_vertexShaderOBJ;
	PixelShader m_pixelShaderOBJ;

	// Pipeline components
	std::shared_ptr<Bind::SwapChain> m_pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pContext;

	std::shared_ptr<Bind::Viewport> m_pViewport;
	std::shared_ptr<Bind::BackBuffer> m_pBackBuffer;
	std::shared_ptr<Bind::RenderTarget> m_pRenderTarget;
	std::shared_ptr<Bind::DepthStencil> m_pDepthStencil;
	std::unordered_map<Bind::Sampler::Type, std::shared_ptr<Bind::Sampler>> m_pSamplerStates;
	std::unordered_map<Side, std::vector<std::shared_ptr<Bind::RenderTarget>>> m_pCubeBuffers; // vector of depth textures for each face
	std::unordered_map<Bind::Rasterizer::Type, std::shared_ptr<Bind::Rasterizer>> m_pRasterizerStates;
};

#endif