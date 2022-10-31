#pragma once
#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#include <d3d11.h>
#include <wrl/client.h>
#include <memory>

template<class T>
class VertexBuffer
{
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	std::vector<XMFLOAT3> vertexPositions;
	UINT stride = sizeof( T );
	UINT vertexCount = 0;
public:
	VertexBuffer() {}
	VertexBuffer( const VertexBuffer<T>& rhs )
	{
		buffer = rhs.buffer;
		stride = rhs.stride;
		vertexCount = rhs.vertexCount;
	}
	VertexBuffer<T>& operator=( const VertexBuffer<T>& rhs )
	{
		buffer = rhs.buffer;
		stride = rhs.stride;
		vertexCount = rhs.vertexCount;
		return *this;
	}
public:
	ID3D11Buffer* Get() const noexcept
	{
		return buffer.Get();
	}
	ID3D11Buffer* const* GetAddressOf() const noexcept
	{
		return buffer.GetAddressOf();
	}
	UINT VertexCount() const noexcept
	{
		return vertexCount;
	}
	const UINT Stride() const noexcept
	{
		return stride;
	}
	const UINT* StridePtr() const noexcept
	{
		return &stride;
	}
	const std::vector<XMFLOAT3>& GetVertexPositions() const noexcept
	{
		return vertexPositions;
	}
	HRESULT Initialize( ID3D11Device* device, T* data, UINT vertexCount )
	{
		if ( buffer.Get() != nullptr )
			buffer.Reset();

		this->vertexCount = vertexCount;

		D3D11_BUFFER_DESC vertexBufferDesc = { 0 };
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = stride * vertexCount;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
		vertexBufferData.pSysMem = data;

		HRESULT hr = device->CreateBuffer( &vertexBufferDesc, &vertexBufferData, buffer.GetAddressOf() );
		return hr;
	}
};

#endif