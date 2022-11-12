#pragma once
#ifndef BACKBUFFER_H
#define BACKBUFFER_H

#include <d3d11.h>
#include <Windows.h>
#include <wrl/client.h>
#include "ErrorLogger.h"
#include "DepthStencil.h"

namespace Bind
{
	class DepthStencil;
	class BackBuffer
	{
	public:
		BackBuffer( ID3D11Device* device, IDXGISwapChain* swapChain )
		{
			try
			{
				Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer;
				HRESULT hr = swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )pBackBuffer.GetAddressOf() );
				COM_ERROR_IF_FAILED( hr, "Failed to create swap chain!" );

				CD3D11_RENDER_TARGET_VIEW_DESC rtvDesc( D3D11_RTV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R8G8B8A8_UNORM );
				hr = device->CreateRenderTargetView( pBackBuffer.Get(), &rtvDesc, backBuffer.GetAddressOf() );
				COM_ERROR_IF_FAILED( hr, "Failed to create render target view!" );
			}
			catch ( COMException& exception )
			{
				ErrorLogger::Log( exception );
				return;
			}
		}
		inline void Bind( ID3D11DeviceContext* context, DepthStencil* depthStencil, float clearColor[4] ) noexcept
		{
			context->OMSetRenderTargets( 1u, backBuffer.GetAddressOf(), depthStencil->GetDepthStencilView() );
			context->ClearRenderTargetView( backBuffer.Get(), clearColor );
		}
		inline void BindNull( ID3D11DeviceContext* context ) noexcept
		{
			Microsoft::WRL::ComPtr<ID3D11RenderTargetView> nullRenderTarget = nullptr;
			context->OMSetRenderTargets( 1u, nullRenderTarget.GetAddressOf(), nullptr );
		}
		inline ID3D11RenderTargetView* GetBackBuffer() noexcept
		{
			return backBuffer.Get();
		}
		inline ID3D11RenderTargetView** GetBackBufferPtr() noexcept
		{
			return backBuffer.GetAddressOf();
		}
	private:
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> backBuffer;
	};
}

#endif