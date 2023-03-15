#pragma once
#ifndef GRAPHICS_H
#define GRAPHICS_H

enum class Side;
#include "Quad.h"
#include "Blender.h"
#include "Stencil.h"
#include "Shaders.h"
#include "Sampler.h"
#include "Viewport.h"
#include "SwapChain.h"
#include "BackBuffer.h"
#include "Rasterizer.h"
#include "DepthStencil.h"
#include "RenderTarget.h"

class Graphics
{
public:
	bool Initialize( HWND hWnd, UINT width, UINT height );
	void BeginFrame();
	void BeginFrameCube( Side side, uint32_t index );
	void BeginFrameCubeInv( Side side, uint32_t index );
	void BeginFrameCubeInvRecursive( uint32_t depth, uint32_t camera, Side side );

	void UpdateRenderStateSkysphere();
	void UpdateRenderStateCube();
	void UpdateRenderStateObject( ID3D11Buffer* const* cbBorder );
	void UpdateRenderStateTexture();

	void BeginRenderSceneToTexture();
	void BindRenderTarget();
	void RenderSceneToTexture(
		ID3D11Buffer* const* cbMotionBlur,
		ID3D11Buffer* const* cbFXAA );
	void EndFrame();

	inline UINT GetWidth() const noexcept { return m_viewWidth; }
	inline UINT GetHeight() const noexcept { return m_viewHeight; }
	inline ID3D11Device* GetDevice() const noexcept { return m_pDevice.Get(); }
	inline ID3D11DeviceContext* GetContext() const noexcept { return m_pContext.Get(); }
	inline Bind::RenderTarget* GetRenderTarget() const noexcept { return &*m_pRenderTarget; }
	inline Bind::DepthStencil* GetDepthStencil() const noexcept { return &*m_pDepthStencil; }
	inline Bind::Stencil* GetStencilState( Side side, Bind::Stencil::Type type ) const noexcept { return &*m_pStencilStates.at( side ).at( type ); }

	inline Bind::RenderTarget* GetCubeBuffer( Side side, uint32_t index ) { return &*m_pCubeBuffers.at( side ).at( index ); }
	inline std::vector<std::shared_ptr<Bind::RenderTarget>> GetCubeBufferSide( Side side ) const noexcept { return m_pCubeBuffers.at( side ); }
	inline std::unordered_map<Side, std::vector<std::shared_ptr<Bind::RenderTarget>>> GetCubeBufferAll( Side side ) const noexcept { return m_pCubeBuffers; }

	inline Bind::RenderTarget* GetCubeInvBuffer( Side side, uint32_t index ) { return &*m_pCubeInvBuffers.at( side ).at( index ); }
	inline std::vector<std::shared_ptr<Bind::RenderTarget>> GetCubeInvBufferSide( Side side ) const noexcept { return m_pCubeInvBuffers.at( side ); }
	inline std::unordered_map<Side, std::vector<std::shared_ptr<Bind::RenderTarget>>> GetCubeInvBufferAll( Side side ) const noexcept { return m_pCubeInvBuffers; }

	inline Bind::RenderTarget* GetCubeInvRecursiveBuffer( uint32_t depth, uint32_t camera, Side side ) { return &*m_pCubeInvRecursiveBuffers.at( depth ).at( camera ).at( side ); }
	inline std::unordered_map<Side, std::shared_ptr<Bind::RenderTarget>> GetCubeInvRecursiveBufferSide( uint32_t depth, uint32_t camera ) const noexcept { return m_pCubeInvRecursiveBuffers.at( depth ).at( camera ); }
	inline std::vector<std::unordered_map<Side, std::shared_ptr<Bind::RenderTarget>>> GetCubeInvRecursiveBufferAllDepth( uint32_t depth ) const noexcept { return m_pCubeInvRecursiveBuffers.at( depth ); }
	inline std::vector<std::vector<std::unordered_map<Side, std::shared_ptr<Bind::RenderTarget>>>> GetCubeInvRecursiveBufferAll() const noexcept { return m_pCubeInvRecursiveBuffers; }

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
	PixelShader m_pixelShaderBD;

	// Pipeline components
	std::shared_ptr<Bind::SwapChain> m_pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pContext;

	std::shared_ptr<Bind::Viewport> m_pViewport;
	std::shared_ptr<Bind::BackBuffer> m_pBackBuffer;
	std::shared_ptr<Bind::RenderTarget> m_pRenderTarget;
	std::shared_ptr<Bind::DepthStencil> m_pDepthStencil;

	std::unordered_map<Bind::Blender::Type, std::shared_ptr<Bind::Blender>> m_pBlenderStates;
	std::unordered_map<Bind::Sampler::Type, std::shared_ptr<Bind::Sampler>> m_pSamplerStates;
	std::unordered_map<Bind::Rasterizer::Type, std::shared_ptr<Bind::Rasterizer>> m_pRasterizerStates;

	std::unordered_map<Side, std::unordered_map<Bind::Stencil::Type, std::shared_ptr<Bind::Stencil>>> m_pStencilStates; // stencils for each face of cube
	std::unordered_map<Side, std::vector<std::shared_ptr<Bind::RenderTarget>>> m_pCubeBuffers; // depth textures for each face of cube
	std::unordered_map<Side, std::vector<std::shared_ptr<Bind::RenderTarget>>> m_pCubeInvBuffers; // depth textures for each face of room (depth 0)
	std::vector<std::vector<std::unordered_map<Side, std::shared_ptr<Bind::RenderTarget>>>> m_pCubeInvRecursiveBuffers; // depth textures for each face of room (depth 1-5)
};

#endif