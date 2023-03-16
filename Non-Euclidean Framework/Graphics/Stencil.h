#pragma once
#ifndef STENCIL_H
#define STENCIL_H

#include <d3d11.h>
#include <Windows.h>
#include <wrl/client.h>
#include "ErrorLogger.h"

namespace Bind
{
	class Stencil
	{
	public:
		enum class Type
		{
			OFF,
			MASK,
			WRITE
		};
		Stencil( ID3D11Device* device, Type type )
		{
			try
			{
				CD3D11_DEPTH_STENCIL_DESC depthStencilStateDesc( CD3D11_DEFAULT{} );

				if ( type == Type::OFF )
				{
					depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
				}
				else if ( type == Type::MASK )
				{
					depthStencilStateDesc.DepthEnable = FALSE;
					depthStencilStateDesc.StencilEnable = TRUE;
					depthStencilStateDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
					depthStencilStateDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
					depthStencilStateDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
					depthStencilStateDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR_SAT;
				}
				else if ( type == Type::WRITE )
				{
					depthStencilStateDesc.StencilEnable = TRUE;
					depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
					depthStencilStateDesc.FrontFace.StencilFunc = D3D11_COMPARISON_LESS;
					depthStencilStateDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
					depthStencilStateDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
					depthStencilStateDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
				}

				HRESULT hr = device->CreateDepthStencilState( &depthStencilStateDesc, pStencil.GetAddressOf() );
				COM_ERROR_IF_FAILED( hr, "Failed to create depth stencil state!" );
			}
			catch ( COMException& exception )
			{
				ErrorLogger::Log( exception );
				return;
			}
		}
		void Clear( ID3D11DeviceContext* context, ID3D11DepthStencilView* dsv ) noexcept
		{
			context->ClearDepthStencilView( dsv, D3D11_CLEAR_STENCIL, 1.0f, 0u );
		}
		void Bind( ID3D11DeviceContext* context ) noexcept
		{
			context->OMSetDepthStencilState( pStencil.Get(), 0u );
		}
	private:
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pStencil;
	};
}

#endif