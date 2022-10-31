#pragma once
#ifndef SAMPLER_H
#define SAMPLER_H

#include <string>
#include <d3d11.h>
#include <Windows.h>
#include <wrl/client.h>
#include "ErrorLogger.h"

namespace Bind
{
	class Sampler
	{
	public:
		enum class Type
		{
			ANISOTROPIC,
			BILINEAR,
			POINT
		};
		Sampler( ID3D11Device* device, Type type, bool reflect = false, UINT slot = 0u ) : type( type ), reflect( reflect ), slot( slot )
		{
			try
			{
				CD3D11_SAMPLER_DESC samplerDesc( CD3D11_DEFAULT{} );
				samplerDesc.Filter = [type]() mutable {
					switch ( type )
					{
					case Type::ANISOTROPIC: return D3D11_FILTER_ANISOTROPIC;
					case Type::POINT: return D3D11_FILTER_MIN_MAG_MIP_POINT;
					default:
					case Type::BILINEAR: return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
					}
				}();
				samplerDesc.AddressU = reflect ? D3D11_TEXTURE_ADDRESS_MIRROR : D3D11_TEXTURE_ADDRESS_WRAP;
				samplerDesc.AddressV = reflect ? D3D11_TEXTURE_ADDRESS_MIRROR : D3D11_TEXTURE_ADDRESS_WRAP;
				samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;

				HRESULT hr = device->CreateSamplerState( &samplerDesc, pSampler.GetAddressOf() );
				COM_ERROR_IF_FAILED( hr, "Failed to create sampler state!" );
			}
			catch ( COMException& exception )
			{
				ErrorLogger::Log( exception );
				return;
			}
		}
		void Bind( ID3D11DeviceContext* context ) noexcept
		{
			context->PSSetSamplers( slot, 1u, pSampler.GetAddressOf() );
		}
	private:
		Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;
		Type type;
		bool reflect;
		UINT slot;
	};
}

#endif