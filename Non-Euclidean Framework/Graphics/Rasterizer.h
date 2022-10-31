#pragma once
#ifndef RASTERIZER_H
#define RASTERIZER_H

#include <d3d11.h>
#include <Windows.h>
#include <wrl/client.h>
#include "ErrorLogger.h"

namespace Bind
{
	class Rasterizer
	{
	public:
		enum class Type
		{
			SOLID,
			SKYSPHERE,
			WIREFRAME
		};
		Rasterizer( ID3D11Device* device, Type type ) : isSolid( isSolid ), isTwoSided( isTwoSided )
		{
			try
			{
				if ( type == Type::SOLID )
				{
					isSolid = true;
					isTwoSided = false;
				}
				else if ( type == Type::SKYSPHERE )
				{
					isSolid = true;
					isTwoSided = true;
				}
				else if ( type == Type::WIREFRAME )
				{
					isSolid = false;
					isTwoSided = true;
				}

				CD3D11_RASTERIZER_DESC rasterizerDesc = CD3D11_RASTERIZER_DESC( CD3D11_DEFAULT{} );
				rasterizerDesc.MultisampleEnable = TRUE;
				rasterizerDesc.AntialiasedLineEnable = TRUE;
				rasterizerDesc.FillMode = isSolid ? D3D11_FILL_SOLID : D3D11_FILL_WIREFRAME;
				rasterizerDesc.CullMode = isTwoSided ? D3D11_CULL_NONE : D3D11_CULL_BACK;
				HRESULT hr = device->CreateRasterizerState( &rasterizerDesc, pRasterizer.GetAddressOf() );
				COM_ERROR_IF_FAILED( hr, "Failed to create rasterizer state!" );
			}
			catch ( COMException& exception )
			{
				ErrorLogger::Log( exception );
				return;
			}
		}
		void Bind( ID3D11DeviceContext* context ) noexcept
		{
			context->RSSetState( pRasterizer.Get() );
		}
		static void DrawSolid( ID3D11DeviceContext* context, UINT indexCount ) noexcept
		{
			Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizer_Solid;
			context->RSSetState( pRasterizer_Solid.Get() );
			context->DrawIndexed( indexCount, 0u, 0u );
		}
	private:
		bool isSolid;
		bool isTwoSided;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizer;
	};
}

#endif