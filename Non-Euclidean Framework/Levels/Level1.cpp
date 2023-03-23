#include "stdafx.h"
#include "Level1.h"

void Level1::OnCreate()
{
	try
	{
        // Initialize constant buffers
        HRESULT hr = m_cbMatrices.Initialize( m_gfx->GetDevice(), m_gfx->GetContext() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'Matrices' constant buffer!" );
        hr = m_cbTextureBorder.Initialize( m_gfx->GetDevice(), m_gfx->GetContext() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'Texture Border' constant buffer!" );

        // Initialize game objects
        hr = m_cylinder.CreateCylinder( m_gfx->GetDevice(), m_gfx->GetContext() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'cylinder' object!" );
        hr = m_cone.CreateCone( m_gfx->GetDevice(), m_gfx->GetContext() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'cone' object!" );
        hr = m_dodecahedron.CreateDodecahedron( m_gfx->GetDevice(), m_gfx->GetContext() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'dodecahedron' object!" );
        hr = m_icosahedron.CreateIcosahedron( m_gfx->GetDevice(), m_gfx->GetContext() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'icosahedron' object!" );
        hr = m_octahedron.CreateOctahedron( m_gfx->GetDevice(), m_gfx->GetContext() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'octahedron' object!" );
        hr = m_teapot.CreateTeapot( m_gfx->GetDevice(), m_gfx->GetContext() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'teapot' object!" );

        // Create scene elements
        hr = m_face.Initialize( m_gfx->GetContext(), m_gfx->GetDevice() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'face' object!" );
        hr = m_stencilCube.Initialize( m_gfx->GetContext(), m_gfx->GetDevice() );
        COM_ERROR_IF_FAILED( hr, "Failed to create 'stencil cube' object!" );

	}
	catch ( COMException& exception )
	{
		ErrorLogger::Log( exception );
        return;
	}
}

void Level1::OnSwitch()
{
}

void Level1::RenderFrame()
{
	auto context = m_gfx->GetContext();

    TextureBorder_CB tbData;
    tbData.TextureBorder = m_fTextureBorder;
    m_cbTextureBorder.data = tbData;
    if ( !m_cbTextureBorder.ApplyChanges() ) return;

    // Draw center stencil cube
    for ( uint32_t i = 0u; i < 6u; i++ )
    {
        m_gfx->GetStencilState( (Side)i, Bind::Stencil::Type::MASK )->Bind( context );
        m_gfx->UpdateRenderStateObject( m_cbTextureBorder.GetAddressOf() );

        ID3D11ShaderResourceView* pTexture = nullptr;
        context->PSSetShaderResources( 0u, 1u, &pTexture );

        switch ( (Side)i )
        {
        case Side::FRONT:
            m_face.SetScale( 1.0f, 1.0f );
            m_face.SetPosition( XMFLOAT3( 0.0f, 0.0f, -1.0f ) );
            m_face.SetRotation( XMFLOAT3( 0.0f, 0.0f, 0.0f ) );
            m_face.Draw( m_cbMatrices, *m_camera );
            m_gfx->GetStencilState( (Side)i, Bind::Stencil::Type::WRITE )->Bind( context );
            m_gfx->UpdateRenderStateObjectInverse();
            m_cylinder.UpdateBuffers( context, m_cbMatrices, *m_camera );
            m_cylinder.Draw( context );
            break;

        case Side::BACK:
            m_face.SetPosition( XMFLOAT3( 0.0f, 0.0f, 1.0f ) );
            m_face.SetRotation( XMFLOAT3( 0.0f, XM_PI, 0.0f ) );
            m_face.Draw( m_cbMatrices, *m_camera );
            m_gfx->GetStencilState( (Side)i, Bind::Stencil::Type::WRITE )->Bind( context );
            m_gfx->UpdateRenderStateObjectInverse();
            m_cone.UpdateBuffers( context, m_cbMatrices, *m_camera );
            m_cone.Draw( context );
            break;

        case Side::LEFT:
            m_face.SetPosition( XMFLOAT3( -1.0f, 0.0f, 0.0f ) );
            m_face.SetRotation( XMFLOAT3( 0.0f, XM_PIDIV2, 0.0f ) );
            m_face.Draw( m_cbMatrices, *m_camera );
            m_gfx->GetStencilState( (Side)i, Bind::Stencil::Type::WRITE )->Bind( context );
            m_gfx->UpdateRenderStateObjectInverse();
            m_dodecahedron.UpdateBuffers( context, m_cbMatrices, *m_camera );
            m_dodecahedron.Draw( context );
            break;

        case Side::RIGHT:
            m_face.SetPosition( XMFLOAT3( 1.0f, 0.0f, 0.0f ) );
            m_face.SetRotation( XMFLOAT3( 0.0f, -XM_PIDIV2, 0.0f ) );
            m_face.Draw( m_cbMatrices, *m_camera );
            m_gfx->GetStencilState( (Side)i, Bind::Stencil::Type::WRITE )->Bind( context );
            m_gfx->UpdateRenderStateObjectInverse();
            m_icosahedron.UpdateBuffers( context, m_cbMatrices, *m_camera );
            m_icosahedron.Draw( context );
            break;

        case Side::TOP:
            m_face.SetPosition( XMFLOAT3( 0.0f, 1.0f, 0.0f ) );
            m_face.SetRotation( XMFLOAT3( XM_PIDIV2, 0.0f, 0.0f ) );
            m_face.Draw( m_cbMatrices, *m_camera );
            m_gfx->GetStencilState( (Side)i, Bind::Stencil::Type::WRITE )->Bind( context );
            m_gfx->UpdateRenderStateObjectInverse();
            m_octahedron.UpdateBuffers( context, m_cbMatrices, *m_camera );
            m_octahedron.Draw( context );
            break;

        case Side::BOTTOM:
            m_face.SetPosition( XMFLOAT3( 0.0f, -1.0f, 0.0f ) );
            m_face.SetRotation( XMFLOAT3( -XM_PIDIV2, 0.0f, 0.0f ) );
            m_face.Draw( m_cbMatrices, *m_camera );
            m_gfx->GetStencilState( (Side)i, Bind::Stencil::Type::WRITE )->Bind( context );
            m_gfx->UpdateRenderStateObjectInverse();
            m_teapot.UpdateBuffers( context, m_cbMatrices, *m_camera );
            m_teapot.Draw( context );
            break;
        }

        // Reset face properties
        m_gfx->GetStencilState( (Side)i, Bind::Stencil::Type::OFF )->Clear( context, m_gfx->GetDepthStencil()->GetDepthStencilView() );
        m_gfx->GetStencilState( (Side)i, Bind::Stencil::Type::MASK )->Clear( context, m_gfx->GetDepthStencil()->GetDepthStencilView() );
        m_gfx->GetStencilState( (Side)i, Bind::Stencil::Type::WRITE )->Clear( context, m_gfx->GetDepthStencil()->GetDepthStencilView() );
        m_gfx->GetStencilState( (Side)i, Bind::Stencil::Type::OFF )->Bind( context );
    }
}

void Level1::Update( const float dt )
{
    // Update objects
    m_cylinder.Update( dt );
    m_octahedron.Update( dt );
    m_teapot.Update( dt );
    m_dodecahedron.Update( dt );
    m_icosahedron.Update( dt );
    m_cone.Update( dt );
}