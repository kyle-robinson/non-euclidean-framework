#pragma once
#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#include <d3d11.h>
#include <Windows.h>
#include <wrl/client.h>
#include "ErrorLogger.h"

namespace Bind
{
	class SwapChain
	{
	public:
        SwapChain( ID3D11DeviceContext** context, ID3D11Device** device, HWND hWnd, int width, int height )
		{
			try
			{
				UINT createDeviceFlags = 0;
            #ifdef _DEBUG
                createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
            #endif

                DXGI_SWAP_CHAIN_DESC sd = { 0 };
                sd.BufferCount = 1;
                sd.BufferDesc.Width = static_cast<UINT>( width );
                sd.BufferDesc.Height = static_cast<UINT>( height );
                sd.BufferDesc.RefreshRate.Numerator = 60;
                sd.BufferDesc.RefreshRate.Denominator = 1;
                sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
                sd.SampleDesc.Count = 1;
                sd.SampleDesc.Quality = 0;
                sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
                sd.BufferCount = 1;
                sd.OutputWindow = hWnd;
                sd.Windowed = TRUE;
                sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
                sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

                HRESULT hr = D3D11CreateDeviceAndSwapChain(
                    nullptr,                    // IDXGI Adapter
                    D3D_DRIVER_TYPE_HARDWARE,   // Driver Type
                    nullptr,                    // Software Module
                    createDeviceFlags,          // Flags for Runtime Layers
                    nullptr,                    // Feature Levels Array
                    0,                          // No. of Feature Levels
                    D3D11_SDK_VERSION,          // SDK Version
                    &sd,                        // Swap Chain Description
                    swapChain.GetAddressOf(),   // Swap Chain Address
                    device,                     // Device Address
                    nullptr,                    // Ptr to Feature Level
                    context                     // Context Address
                );
                COM_ERROR_IF_FAILED( hr, "Failed to create Device and Swap Chain!" );
			}
			catch ( COMException& exception )
			{
				ErrorLogger::Log( exception );
				return;
			}
        }
        IDXGISwapChain* GetSwapChain() noexcept
        {
            return swapChain.Get();
        }
	private:
        Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	};
}

#endif