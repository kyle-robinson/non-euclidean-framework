#pragma once
#ifndef BLENDER_H
#define BLENDER_H

#include <d3d11.h>
#include <Windows.h>
#include <wrl/client.h>
#include "ErrorLogger.h"

namespace Bind
{
	class Blender
	{
	public:
		enum class Type
		{
			BASIC,
			COLOR
		};
		Blender( ID3D11Device* device, Type type )
		{
			try
			{
				D3D11_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc = { 0 };
				renderTargetBlendDesc.BlendEnable = TRUE;
				renderTargetBlendDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
				renderTargetBlendDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
				renderTargetBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
				renderTargetBlendDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
				renderTargetBlendDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
				renderTargetBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
				renderTargetBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

				if ( type == Type::COLOR )
				{
					renderTargetBlendDesc.SrcBlend = D3D11_BLEND_SRC_COLOR;
					renderTargetBlendDesc.DestBlend = D3D11_BLEND_INV_SRC_COLOR;
				}

				D3D11_BLEND_DESC blendDesc = { 0 };
				blendDesc.RenderTarget[0] = renderTargetBlendDesc;
				HRESULT hr = device->CreateBlendState( &blendDesc, blendState.GetAddressOf() );
				COM_ERROR_IF_FAILED( hr, "Failed to create Blend State!" );
			}
			catch ( COMException& exception )
			{
				ErrorLogger::Log( exception );
				return;
			}
		}
		void Bind( ID3D11DeviceContext* context ) noexcept
		{
			context->OMSetBlendState( blendState.Get(), NULL, 0xFFFFFFFF );
		}
	private:
		Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;
	};
}

#endif