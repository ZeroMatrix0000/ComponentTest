/*
 * FileName:     Text.h
 * Author:       Takao Hayata
 * Last Updated: 2026/09/30
 *
 * テキスト
 */

#pragma once

#include "DecorationString.h"
#include "Scripts/Commons/Components/Component.h"

namespace Renderings
{
	class Canvas;
	class ITextRenderer;

	// テキスト
	class Text : public Component
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		Text(const ComponentDesc& desc, ITextRenderer* pITextRenderer);
		// デストラクタ
		~Text();

		// 初期化処理
		void Initalize(const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder) override;

		// 文字列を設定
		void SetStr(const std::wstring& str) { m_str.SetStr(str, m_isDecoration); }

		// フォント名を設定
		void SetFontName(const std::wstring& fontName)          { m_fontName = fontName; }
		// フォントサイズを設定
		void SetFontSize(float fontSize)                        { m_fontSize = fontSize; }
		// フォント色を設定
		void SetFontColor(const Math::Color& color)             { m_fontColor = D2D1::ColorF{ color.x, color.y, color.z, color.w }; }
		// フォント色を設定
		void SetFontColor(const DirectX::XMVECTORF32& color)    { m_fontColor = D2D1::ColorF{ color.f[0], color.f[1], color.f[2], color.f[3] }; }

		// テキスト配置を設定
		void SetTextAlignment(DWRITE_TEXT_ALIGNMENT textAlignment)                { m_textAlignment = textAlignment; }
		// 段落配置を設定
		void SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT paragraphAlignment) { m_paragraphAlignment = paragraphAlignment; }

		// アウトラインの太さを設定
		void SetOutlineWidth(float outlineWidth) { m_outlineWidth = outlineWidth; }
		// アウトライン色を設定
		void SetOutlineColor(const Math::Color& color) { m_outlineColor = D2D1::ColorF{ color.x, color.y, color.z, color.w }; }
		// アウトライン色を設定
		void SetOutlineColor(const DirectX::XMVECTORF32& color) { m_outlineColor = D2D1::ColorF{ color.f[0], color.f[1], color.f[2], color.f[3] }; }

		// レイヤー順を設定
		void SetOrderInLayer(int orderInLayer) { m_orderInLayer = orderInLayer; }

		// キャンバスを設定
		void SetCanvas(const Canvas& canvas) { m_pCanvas = &canvas; }

		// 文字列を取得
		const std::wstring& GetStr() const { return m_str.GetStr(); }

		// デコレーション文字列を取得
		const DecorationString& GetDecorationString(ITextRenderer&) const { return m_str; }

		// フォント名を取得
		const std::wstring& GetFontName() const { return m_fontName; }
		// フォントサイズを取得
		float GetFontSize() const { return m_fontSize; }
		// 色を取得
		Math::Color GetFontColor() const { return Math::Color{ m_fontColor.r, m_fontColor.g, m_fontColor.b, m_fontColor.a }; }
		// 色を取得
		const D2D1::ColorF& GetD2D1FontColor() const { return m_fontColor; }

		// テキスト配置を取得
		DWRITE_TEXT_ALIGNMENT      GetTextAlignment()      const { return m_textAlignment; }
		// 段落配置を取得
		DWRITE_PARAGRAPH_ALIGNMENT GetParagraphAlignment() const { return m_paragraphAlignment; }

		// 行間を取得
		float GetLineSpace() const { return m_lineSpace; }
		
		// アウトラインの太さを取得
		float GetOutlineWidth() const { return m_outlineWidth; }
		// アウトライン色を取得
		Math::Color GetOutlineColor() const { return Math::Color{ m_outlineColor.r, m_outlineColor.g, m_outlineColor.b, m_outlineColor.a }; }
		// アウトライン色を取得
		const D2D1::ColorF& GetD2D1OutlineColor() const { return m_outlineColor; }

		// レイヤー順を取得
		int GetOrderInLayer() const { return m_orderInLayer; }

		// キャンバスのポインタを取得
		const Canvas* GetPCanvas() const { return m_pCanvas; }

	private:


		/* メンバ変数 */

		// 文字列
		DecorationString m_str;
		// デコレーションするか
		bool m_isDecoration;

		// フォント名
		std::wstring m_fontName;
		// フォントサイズ
		float m_fontSize;
		// 色
		D2D1::ColorF m_fontColor;

		// テキスト配置
		DWRITE_TEXT_ALIGNMENT      m_textAlignment;
		// 段落配置
		DWRITE_PARAGRAPH_ALIGNMENT m_paragraphAlignment;

		// 行間
		float m_lineSpace;

		// アウトラインの太さ
		float m_outlineWidth;
		// アウトライン色
		D2D1::ColorF m_outlineColor;

		// レイヤー順
		int m_orderInLayer;

		// キャンバスのポインタ
		const Canvas* m_pCanvas;

		// テキスト描画インタフェースのポインタ
		ITextRenderer* m_pITextRenderer;

	};
}
