#pragma once
#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <d3d11.h>
#include <Windows.h>
#include <wrl/client.h>

namespace Bind
{
	class Viewport
	{
	public:
		Viewport( ID3D11DeviceContext* context, int width, int height )
		{
			viewportDesc = CD3D11_VIEWPORT( 0.0f, 0.0f, static_cast<FLOAT>( width ), static_cast<FLOAT>( height ) );
			context->RSSetViewports( 1u, &viewportDesc );
		}
	private:
		CD3D11_VIEWPORT viewportDesc = {};
	};
}

#endif