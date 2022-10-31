#pragma once
#ifndef CONSTANTBUFFER_H
#define CONSTANTBUFFER_H

#include <d3d11.h>
#include <wrl/client.h>
#include "ErrorLogger.h"

template<class T>
class ConstantBuffer
{
private:
	ConstantBuffer( const ConstantBuffer<T>& rhs ) {}
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	ID3D11DeviceContext* context = nullptr;
public:
	ConstantBuffer() {}
	T data;
	ID3D11Buffer* Get() const noexcept
	{
		return buffer.Get();
	}
	ID3D11Buffer* const* GetAddressOf() const noexcept
	{
		return buffer.GetAddressOf();
	}
	HRESULT Initialize( ID3D11Device* device, ID3D11DeviceContext* context )
	{
		if ( buffer.Get() != nullptr )
			buffer.Reset();
		
		this->context = context;

		D3D11_BUFFER_DESC constantBufferDesc = { 0 };
		constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		constantBufferDesc.ByteWidth = static_cast<UINT>( sizeof( T ) + ( 16 - ( sizeof( T ) % 16 ) ) );
		constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		constantBufferDesc.MiscFlags = 0;
		constantBufferDesc.StructureByteStride = 0;

		HRESULT hr = device->CreateBuffer( &constantBufferDesc, NULL, buffer.GetAddressOf() );
		return hr;
	}
	bool ApplyChanges()
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = context->Map( buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource );
		if ( FAILED( hr ) )
		{
			ErrorLogger::Log( hr, "Failed to map constant buffer!" );
			return false;
		}
		CopyMemory( mappedResource.pData, &data, sizeof( T ) );
		context->Unmap( buffer.Get(), 0 );
		return true;
	}
};

#endif