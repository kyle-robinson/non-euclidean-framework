#include "stdafx.h"
#include "Graphics.h"

bool Graphics::Initialize( HWND hWnd, UINT width, UINT height )
{
	m_viewWidth = width;
	m_viewHeight = height;

	InitializeDirectX( hWnd );

	if ( !InitializeShaders() )
		return false;

	if ( !InitializeRTT() )
		return false;
	
	return true;
}

void Graphics::InitializeDirectX( HWND hWnd )
{
	m_pSwapChain = std::make_shared<Bind::SwapChain>( m_pContext.GetAddressOf(), m_pDevice.GetAddressOf(), hWnd, m_viewWidth, m_viewHeight );
    m_pBackBuffer = std::make_shared<Bind::BackBuffer>( m_pDevice.Get(), m_pSwapChain->GetSwapChain() );
	for ( uint32_t i = 0u; i < RENDER_DEPTH; i++ )
		m_pCubeBuffers.push_back( std::make_shared<Bind::RenderTarget>( m_pDevice.Get(), m_viewWidth, m_viewHeight ) );
    m_pRenderTarget = std::make_shared<Bind::RenderTarget>( m_pDevice.Get(), m_viewWidth, m_viewHeight );
    m_pDepthStencil = std::make_shared<Bind::DepthStencil>( m_pDevice.Get(), m_viewWidth, m_viewHeight );
	m_pViewport = std::make_shared<Bind::Viewport>( m_pContext.Get(), m_viewWidth, m_viewHeight );
    
    m_pRasterizerStates.emplace( Bind::Rasterizer::Type::SOLID, std::make_shared<Bind::Rasterizer>( m_pDevice.Get(), Bind::Rasterizer::Type::SOLID ) );
    m_pRasterizerStates.emplace( Bind::Rasterizer::Type::SKYSPHERE, std::make_shared<Bind::Rasterizer>( m_pDevice.Get(), Bind::Rasterizer::Type::SKYSPHERE ) );
    m_pRasterizerStates.emplace( Bind::Rasterizer::Type::WIREFRAME, std::make_shared<Bind::Rasterizer>( m_pDevice.Get(), Bind::Rasterizer::Type::WIREFRAME ) );

    m_pSamplerStates.emplace( Bind::Sampler::Type::ANISOTROPIC, std::make_shared<Bind::Sampler>( m_pDevice.Get(), Bind::Sampler::Type::ANISOTROPIC ) );
	m_pSamplerStates.emplace( Bind::Sampler::Type::BILINEAR, std::make_shared<Bind::Sampler>( m_pDevice.Get(), Bind::Sampler::Type::BILINEAR ) );
	m_pSamplerStates.emplace( Bind::Sampler::Type::POINT, std::make_shared<Bind::Sampler>( m_pDevice.Get(), Bind::Sampler::Type::POINT ) );

	m_pSamplerStates[Bind::Sampler::Type::ANISOTROPIC]->Bind( m_pContext.Get() );
    m_pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	RENDER_DEPTH = 3u;
}

bool Graphics::InitializeShaders()
{
	try
	{
		// Define input layout for cube
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		// Create the cube shaders
		HRESULT hr = m_vertexShader.Initialize( m_pDevice, L"Resources\\Shaders\\shader.fx", layout, ARRAYSIZE( layout ) );
		COM_ERROR_IF_FAILED( hr, "Failed to create cube vertex shader!" );
		hr = m_pixelShader.Initialize( m_pDevice, L"Resources\\Shaders\\shader.fx" );
		COM_ERROR_IF_FAILED( hr, "Failed to create cube pixel shader!" );

		// Define input layout for models
		D3D11_INPUT_ELEMENT_DESC layoutOBJ[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		// Create the model shaders
		hr = m_vertexShaderOBJ.Initialize( m_pDevice, L"Resources\\Shaders\\shaderOBJ.fx", layoutOBJ, ARRAYSIZE( layoutOBJ ) );
		COM_ERROR_IF_FAILED( hr, "Failed to create skysphere vertex shader!" );
		hr = m_pixelShaderOBJ.Initialize( m_pDevice, L"Resources\\Shaders\\shaderOBJ.fx" );
		COM_ERROR_IF_FAILED( hr, "Failed to create skysphere pixel shader!" );

		// Define input layout for textures
		D3D11_INPUT_ELEMENT_DESC layoutTEX[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		// Create the texture shaders
		hr = m_vertexShaderTEX.Initialize( m_pDevice, L"Resources\\Shaders\\shaderTEX.fx", layoutTEX, ARRAYSIZE( layoutTEX ) );
		COM_ERROR_IF_FAILED( hr, "Failed to create texture vertex shader!" );
		hr = m_pixelShaderTEX.Initialize( m_pDevice, L"Resources\\Shaders\\shaderTEX.fx" );
		COM_ERROR_IF_FAILED( hr, "Failed to create texture pixel shader!" );

		// Define input layout for quad
		D3D11_INPUT_ELEMENT_DESC layoutPP[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		// Create the quad shaders
		hr = m_vertexShaderPP.Initialize( m_pDevice, L"Resources\\Shaders\\shaderPP.fx", layoutPP, ARRAYSIZE( layoutPP ) );
		COM_ERROR_IF_FAILED( hr, "Failed to create quad vertex shader!" );
		hr = m_pixelShaderPP.Initialize( m_pDevice, L"Resources\\Shaders\\shaderPP.fx" );
		COM_ERROR_IF_FAILED( hr, "Failed to create quad pixel shader!" );
	}
	catch ( COMException& exception )
	{
		ErrorLogger::Log( exception );
		return false;
	}

	return true;
}

bool Graphics::InitializeRTT()
{
	// Initialize quad for post-processing
	if ( !m_quad.Initialize( m_pDevice.Get() ) )
		return false;
	return true;
}

void Graphics::BeginFrame()
{
	// Clear render target/depth stencil
    m_pRenderTarget->Bind( m_pContext.Get(), m_pDepthStencil.get(), m_clearColor );
    m_pDepthStencil->ClearDepthStencil( m_pContext.Get() );
}

void Graphics::BeginFrameCube( uint32_t index )
{
	// Clear render target/depth stencil
    m_pCubeBuffers.at( index )->Bind( m_pContext.Get(), m_pDepthStencil.get(), m_clearColor );
    m_pDepthStencil->ClearDepthStencil( m_pContext.Get() );
}

void Graphics::UpdateRenderStateSkysphere()
{
	// Set render state for skysphere
    m_pRasterizerStates[Bind::Rasterizer::Type::SKYSPHERE]->Bind( m_pContext.Get() );
	Shaders::BindShaders( m_pContext.Get(), m_vertexShaderOBJ, m_pixelShaderOBJ );
}

void Graphics::UpdateRenderStateCube()
{
	// Set render state
    m_pRasterizerStates[Bind::Rasterizer::Type::SOLID]->Bind( m_pContext.Get() );
    Shaders::BindShaders( m_pContext.Get(), m_vertexShader, m_pixelShader );
}

void Graphics::UpdateRenderStateObject()
{
	// Set default render state for objects
    m_pRasterizerStates[Bind::Rasterizer::Type::SOLID]->Bind( m_pContext.Get() );
    Shaders::BindShaders( m_pContext.Get(), m_vertexShaderOBJ, m_pixelShaderOBJ );
}

void Graphics::UpdateRenderStateTexture()
{
	// Set default render state for objects
    m_pRasterizerStates[Bind::Rasterizer::Type::SOLID]->Bind( m_pContext.Get() );
    Shaders::BindShaders( m_pContext.Get(), m_vertexShaderTEX, m_pixelShaderTEX );
}

void Graphics::BeginRenderSceneToTexture()
{
	// Bind new render target
	m_pBackBuffer->Bind( m_pContext.Get(), m_pDepthStencil.get(), m_clearColor );
}

void Graphics::RenderSceneToTexture( ID3D11Buffer* const* cbMotionBlur, ID3D11Buffer* const* cbFXAA )
{
	// Render fullscreen texture to new render target
	Shaders::BindShaders( m_pContext.Get(), m_vertexShaderPP, m_pixelShaderPP );
	m_pContext->PSSetConstantBuffers( 0u, 1u, cbMotionBlur );	
	m_pContext->PSSetConstantBuffers( 1u, 1u, cbFXAA );	
	m_quad.SetupBuffers( m_pContext.Get() );
	m_pContext->PSSetShaderResources( 0u, 1u, m_pRenderTarget->GetShaderResourceViewPtr() );
	m_pContext->PSSetShaderResources( 1u, 1u, m_pDepthStencil->GetShaderResourceViewPtr() );
	Bind::Rasterizer::DrawSolid( m_pContext.Get(), m_quad.GetIndexBuffer().IndexCount() ); // always draw as solid
}

void Graphics::EndFrame()
{
	// Unbind render target
	m_pRenderTarget->BindNull( m_pContext.Get() );
	for ( uint32_t i = 0u; i < RENDER_DEPTH; i++ )
		m_pCubeBuffers.at( i )->BindNull( m_pContext.Get() );
	m_pBackBuffer->BindNull( m_pContext.Get() );

	// Present frame
	HRESULT hr = m_pSwapChain->GetSwapChain()->Present( 1u, NULL );
	if ( FAILED( hr ) )
	{
		hr == DXGI_ERROR_DEVICE_REMOVED ?
			ErrorLogger::Log( m_pDevice->GetDeviceRemovedReason(), "Swap Chain. Graphics device removed!" ) :
			ErrorLogger::Log( hr, "Swap Chain failed to render frame!" );
		exit( -1 );
	}
}