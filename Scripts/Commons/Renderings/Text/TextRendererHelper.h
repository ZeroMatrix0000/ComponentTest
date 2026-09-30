/*
 * FileName:     TextRendererHelper.h
 * Author:       Takao Hayata
 * Last Updated: 2026/09/30
 *
 * テキスト描画のヘルパー関数群
 */

#pragma once

#include "Scripts/Commons/Systems/OnlyOne.h"

namespace Renderings
{
	class Text;
	class TextRenderer;
	class DecorationTextRenderer;

	// テキスト描画のヘルパー関数群
	struct TextRenderHelper : Systems::OnlyOne
	{

		/* メンバ関数 */

		// コンストラクタ
		TextRenderHelper(TextRenderer& renderer);

		// 長方形をキャンバス情報とピボットとアンカーに沿って移動
		void AdjustRect
		(
			const Math::Vector2&    canvasSize,
			float                   canvasRatio,
			Utility::AlignmentPoint pivot,
			Utility::AlignmentPoint anchor,
			Math::Rect*             pRect
		);

		// テキストフォーマットを生成
		void CreateTextFormat
		(
			const std::wstring&        fontName,
			float                      fontSize,
			float                      lineSpace,
			DWRITE_TEXT_ALIGNMENT      textAlignment,
			DWRITE_PARAGRAPH_ALIGNMENT paragraphAlignment,
			IDWriteFactory8*           pDWriteFactory,
			IDWriteFontCollection3*    pFontCollection,
			IDWriteTextFormat**        ppTextFormat
		);
		
		// 文字を描画
		void DrawTextLayout
		(
			const Math::Vector2&    position,
			ID2D1DeviceContext7*    pContext,
			IDWriteFactory8*        pDWriteFactory,
			IDWriteFontCollection3* pFontCollection,
			IDWriteTextLayout*      pTextLayout,
			ID2D1SolidColorBrush*   pBrush,
			DecorationTextRenderer* pDecorationRenderer,
			const Text*             pText
		);


		/* メンバ変数 */

		// テキスト描画
		TextRenderer& m_refRenderer;

	};
}
