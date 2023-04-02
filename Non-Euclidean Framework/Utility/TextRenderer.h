#pragma once
#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H

#include <dxtk/SpriteFont.h>
#include <dxtk/SpriteBatch.h>

/// <summary>
/// Uses SpriteFont and SpriteBatch to draw strings to the screen.
/// Font to be used can be chagned within this classes constructor.
/// </summary>
class TextRenderer
{
public:
	TextRenderer() {}
	~TextRenderer() {}

	void Initialize( ID3D11Device* device, ID3D11DeviceContext* context );
	void RenderString( const std::string& text, XMFLOAT2 position, XMVECTORF32 color, bool outline );
	void UpdateViewport( D3D11_VIEWPORT& viewport );

	inline XMFLOAT2 GetScale() const noexcept { return m_vScale; }
	inline void SetScale( XMFLOAT2 scale ) noexcept { m_vScale = scale; }

	inline SpriteBatch* GetSpriteBatch() const noexcept { return m_spriteBatch.get(); }
	inline SpriteFont* GetSpriteFont() const noexcept{ return m_spriteFont.get(); }
private:
	void DrawString( const std::wstring& text, XMFLOAT2 position, XMVECTORF32 color, bool outline );

	XMFLOAT2 m_vScale = { 1.0f, 1.0f };
	std::unique_ptr<SpriteFont> m_spriteFont;
	std::unique_ptr<SpriteBatch> m_spriteBatch;
};

#endif