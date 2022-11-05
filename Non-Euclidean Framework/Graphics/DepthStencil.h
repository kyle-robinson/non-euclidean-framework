#pragma once
#ifndef DEPTHSTENCIL_H
#define DEPTHSTENCIL_H

#include <d3d11.h>
#include <Windows.h>
#include <wrl/client.h>
#include "ErrorLogger.h"

extern UINT MAX_QUALITY;
extern UINT SAMPLE_COUNT;

namespace Bind
{
	class DepthStencil
	{
	public:
		DepthStencil( ID3D11Device* device, int width, int height )
		{
			try
			{
				// create texture resource
				D3D11_TEXTURE2D_DESC textureDesc = { 0 };
				textureDesc.Width = width;
				textureDesc.Height = height;
				textureDesc.MipLevels = 1u;
				textureDesc.ArraySize = 1u;
				textureDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
				textureDesc.SampleDesc.Count = SAMPLE_COUNT;
				textureDesc.SampleDesc.Quality = MAX_QUALITY;
				textureDesc.Usage = D3D11_USAGE_DEFAULT;
				textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
				textureDesc.CPUAccessFlags = 0u;
				textureDesc.MiscFlags = 0u;

				// create resource data
				uint32_t* initData = new uint32_t[width * height];
				ZeroMemory(initData, sizeof(uint32_t) * width * height);

				D3D11_SUBRESOURCE_DATA data;
				ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
				data.pSysMem = initData;
				data.SysMemPitch = sizeof(uint32_t) * width;
				data.SysMemSlicePitch = 0;

				// create texture
				Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
				HRESULT hr = device->CreateTexture2D( &textureDesc, &data, depthStencilBuffer.GetAddressOf() );
				COM_ERROR_IF_FAILED( hr, "Failed to create depth stencil texture!" );

				// create resource view on texture
				D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
				srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				srvDesc.Texture2D.MostDetailedMip = 0u;
				srvDesc.Texture2D.MipLevels = 1u;
				hr = device->CreateShaderResourceView( depthStencilBuffer.Get(), &srvDesc, shaderResourceView.GetAddressOf() );
				COM_ERROR_IF_FAILED( hr, "Failed to create Shader Resource View!" );

				// create depth stencil view
				D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
				depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
				depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
				hr = device->CreateDepthStencilView( depthStencilBuffer.Get(), &depthStencilViewDesc, depthStencilView.GetAddressOf() );
				COM_ERROR_IF_FAILED( hr, "Failed to create depth stencil view!" );
			}
			catch ( COMException& exception )
			{
				ErrorLogger::Log( exception );
				return;
			}
		}
		inline void ClearDepthStencil( ID3D11DeviceContext* context ) noexcept
		{
			context->ClearDepthStencilView( depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u );
		}
		inline ID3D11DepthStencilView* GetDepthStencilView() noexcept
		{
			return depthStencilView.Get();
		}
		inline ID3D11DepthStencilView** GetDepthStencilViewPtr() noexcept
		{
			return depthStencilView.GetAddressOf();
		}
		inline ID3D11ShaderResourceView* GetShaderResourceView() noexcept
		{
			return shaderResourceView.Get();
		}
		inline ID3D11ShaderResourceView** GetShaderResourceViewPtr() noexcept
		{
			return shaderResourceView.GetAddressOf();
		}
	private:
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	};
}

#endif