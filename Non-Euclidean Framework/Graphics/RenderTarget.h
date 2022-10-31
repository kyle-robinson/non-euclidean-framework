#pragma once
#ifndef RENDERTARGET_H
#define RENDERTARGET_H

#include <d3d11.h>
#include <Windows.h>
#include <wrl/client.h>
#include "ErrorLogger.h"
#include "DepthStencil.h"

namespace Bind
{
	class DepthStencil;
	class RenderTarget
	{
	public:
		RenderTarget( ID3D11Device* device, IDXGISwapChain* swapChain )
		{
			try
			{
				Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer;
				HRESULT hr = swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )pBackBuffer.GetAddressOf() );
				COM_ERROR_IF_FAILED( hr, "Failed to create swap chain!" );
				hr = device->CreateRenderTargetView( pBackBuffer.Get(), nullptr, backBuffer.GetAddressOf() );
				COM_ERROR_IF_FAILED( hr, "Failed to create render target view!" );
			}
			catch ( COMException& exception )
			{
				ErrorLogger::Log( exception );
				return;
			}
		}
		RenderTarget( ID3D11Device* device, int width, int height )
		{
			try
			{
				// create texture resource
				D3D11_TEXTURE2D_DESC textureDesc = { 0 };
				textureDesc.Width = width;
				textureDesc.Height = height;
				textureDesc.MipLevels = 1;
				textureDesc.ArraySize = 1;
				textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				textureDesc.SampleDesc.Count = 1u;
				textureDesc.SampleDesc.Quality = 0u;
				textureDesc.Usage = D3D11_USAGE_DEFAULT;
				textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
				textureDesc.CPUAccessFlags = 0;
				textureDesc.MiscFlags = 0;
				Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
				HRESULT hr = device->CreateTexture2D( &textureDesc, nullptr, pTexture.GetAddressOf() );
				COM_ERROR_IF_FAILED( hr, "Failed to create Texture for Render Target!" );

				// create resource view on texture
				D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
				srvDesc.Format = textureDesc.Format;
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				srvDesc.Texture2D.MostDetailedMip = 0;
				srvDesc.Texture2D.MipLevels = 1;
				hr = device->CreateShaderResourceView( pTexture.Get(), &srvDesc, shaderResourceView.GetAddressOf() );
				COM_ERROR_IF_FAILED( hr, "Failed to create Shader Resource View!" );

				// create the target view on the texture
				D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
				rtvDesc.Format = textureDesc.Format;
				rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
				rtvDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };
				hr = device->CreateRenderTargetView( pTexture.Get(), &rtvDesc, renderTargetView.GetAddressOf() );
				COM_ERROR_IF_FAILED( hr, "Failed to create Render Target View with Texture!" );
			}
			catch ( COMException& exception )
			{
				ErrorLogger::Log( exception );
				return;
			}
		}
		void BindAsBuffer( ID3D11DeviceContext* context, DepthStencil* depthStencil, float clearColor[4] ) noexcept
		{
			context->OMSetRenderTargets( 1, backBuffer.GetAddressOf(), depthStencil->GetDepthStencilView() );
			context->ClearRenderTargetView( backBuffer.Get(), clearColor );
		}
		void BindAsTexture( ID3D11DeviceContext* context, DepthStencil* depthStencil, float clearColor[4] ) noexcept
		{
			context->OMSetRenderTargets( 1u, renderTargetView.GetAddressOf(), depthStencil->GetDepthStencilView() );
			context->ClearRenderTargetView( renderTargetView.Get(), clearColor );
		}
		void BindAsNull( ID3D11DeviceContext* context ) noexcept
		{
			Microsoft::WRL::ComPtr<ID3D11RenderTargetView> nullRenderTarget = nullptr;
			context->OMSetRenderTargets( 1u, nullRenderTarget.GetAddressOf(), nullptr );
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> nullShaderResourceView = nullptr;
			context->PSSetShaderResources( 0u, 1u, nullShaderResourceView.GetAddressOf() );
		}
		ID3D11ShaderResourceView* GetShaderResourceView() noexcept
		{
			return shaderResourceView.Get();
		}
		ID3D11ShaderResourceView** GetShaderResourceViewPtr() noexcept
		{
			return shaderResourceView.GetAddressOf();
		}
	private:
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> backBuffer;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	};
}

#endif