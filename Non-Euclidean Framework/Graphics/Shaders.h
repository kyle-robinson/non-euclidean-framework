#pragma once
#ifndef SHADERS_H
#define SHADERS_H

class VertexShader;
class PixelShader;

class Shaders
{
public:
	static void BindShaders( ID3D11DeviceContext* context, VertexShader& vs, PixelShader& ps ) noexcept;
protected:
	HRESULT  CompileShaderFromFile(
		std::wstring szFileName,
		LPCSTR szEntryPoint,
		LPCSTR szShaderModel,
		ID3DBlob** ppBlobOut
	);
};

class VertexShader : Shaders
{
public:
	HRESULT Initialize(
		Microsoft::WRL::ComPtr<ID3D11Device>& device,
		std::wstring shaderPath,
		D3D11_INPUT_ELEMENT_DESC* layoutDesc,
		UINT numElements
	);
	ID3D11VertexShader* GetShader() const noexcept { return shader.Get(); };
	ID3D10Blob* GetBuffer() const noexcept { return shaderBuffer.Get(); };
	ID3D11InputLayout* GetInputLayout() const noexcept { return inputLayout.Get(); };
private:
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> shader;
	Microsoft::WRL::ComPtr<ID3D10Blob> shaderBuffer;
};

class PixelShader : Shaders
{
public:
	HRESULT Initialize( Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderPath );
	ID3D11PixelShader* GetShader() const noexcept { return shader.Get(); }
	ID3D10Blob* GetBlob() const noexcept { return shaderBuffer.Get(); };
private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> shader;
	Microsoft::WRL::ComPtr<ID3D10Blob> shaderBuffer;
};

#endif