/*
 * FileName:     TextRendererHelper.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/09/30
 *
 * テキスト描画のヘルパー関数群
 */

#include "Pch.h"
#include "TextRendererHelper.h"

#include "Text.h"
#include "TextRenderer.h"

// コンストラクタ
Renderings::TextRenderHelper::TextRenderHelper(TextRenderer& renderer)
	: OnlyOne{ typeid(TextRenderHelper) }
	, m_refRenderer{ renderer }
{
}

// 長方形をキャンバスサイズとピボットとアンカーに沿って移動
void Renderings::TextRenderHelper::AdjustRect(const Math::Vector2& canvasSize, float canvasRatio, Utility::AlignmentPoint pivot, Utility::AlignmentPoint anchor, Math::Rect* pRect)
{
	// ピボットに合わせて長方形を移動
	switch (pivot)
	{
	case Utility::AlignmentPoint::TopLeft:
		pRect->position.x += pRect->size.x / 2.0f;
		pRect->position.y += pRect->size.y / 2.0f;
		break;
	case Utility::AlignmentPoint::TopCenter:
		pRect->position.y += pRect->size.y / 2.0f;
		break;
	case Utility::AlignmentPoint::TopRight:
		pRect->position.x -= pRect->size.x / 2.0f;
		pRect->position.y += pRect->size.y / 2.0f;
		break;
	case Utility::AlignmentPoint::MiddleLeft:
		pRect->position.x += pRect->size.x / 2.0f;
		break;
	case Utility::AlignmentPoint::MiddleCenter:
		break;
	case Utility::AlignmentPoint::MiddleRight:
		pRect->position.x -= pRect->size.x / 2.0f;
		break;
	case Utility::AlignmentPoint::BottomLeft:
		pRect->position.x += pRect->size.x / 2.0f;
		pRect->position.y -= pRect->size.y / 2.0f;
		break;
	case Utility::AlignmentPoint::BottomCenter:
		pRect->position.y -= pRect->size.y / 2.0f;
		break;
	case Utility::AlignmentPoint::BottomRight:
		pRect->position.x -= pRect->size.x / 2.0f;
		pRect->position.y -= pRect->size.y / 2.0f;
		break;
	default:
		break;
	}
	// アンカーに合わせて長方形を移動
	switch (anchor)
	{
	case Utility::AlignmentPoint::TopCenter:
		pRect->position.x += canvasSize.x / 2.0f;
		break;
	case Utility::AlignmentPoint::TopRight:
		pRect->position.x += canvasSize.x;
		break;
	case Utility::AlignmentPoint::MiddleLeft:
		pRect->position.y += canvasSize.y / 2.0f;
		break;
	case Utility::AlignmentPoint::MiddleCenter:
		pRect->position.x += canvasSize.x / 2.0f;
		pRect->position.y += canvasSize.y / 2.0f;
		break;
	case Utility::AlignmentPoint::MiddleRight:
		pRect->position.x += canvasSize.x;
		pRect->position.y += canvasSize.y / 2.0f;
		break;
	case Utility::AlignmentPoint::BottomLeft:
		pRect->position.y += canvasSize.y;
		break;
	case Utility::AlignmentPoint::BottomCenter:
		pRect->position.x += canvasSize.x / 2.0f;
		pRect->position.y += canvasSize.y;
		break;
	case Utility::AlignmentPoint::BottomRight:
		pRect->position.x += canvasSize.x;
		pRect->position.y += canvasSize.y;
		break;
	default:
		break;
	}

	// 表示倍率を適用
	pRect->position *= canvasRatio;
	pRect->size *= canvasRatio;
}

// テキストフォーマットを生成
void Renderings::TextRenderHelper::CreateTextFormat(const std::wstring& fontName, float fontSize, float lineSpace, DWRITE_TEXT_ALIGNMENT textAlignment, DWRITE_PARAGRAPH_ALIGNMENT paragraphAlignment, IDWriteFactory8* pDWriteFactory, IDWriteFontCollection3* pFontCollection, IDWriteTextFormat** ppTextFormat)
{
	pDWriteFactory->CreateTextFormat
	(
		fontName.c_str(),
		pFontCollection,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		fontSize,
		L"ja-jp",
		ppTextFormat
	);

	// 行の上端とベースラインの距離
	float baseline{};
	switch (paragraphAlignment)
	{
	case DWRITE_PARAGRAPH_ALIGNMENT_NEAR:
		// 行の上端に文字の上端ががつくようにする
		baseline = fontSize;
		break;
	case DWRITE_PARAGRAPH_ALIGNMENT_CENTER:
		// 行の中心に文字の中心がつくようにする
		baseline = fontSize * (0.4f + lineSpace / 2.0f);
		break;
	case DWRITE_PARAGRAPH_ALIGNMENT_FAR:
		// 行の下端に文字の下端がつくようにする
		baseline = fontSize * (lineSpace - 0.2f);
		break;
	default:
		break;
	}

	// 行間を設定
	(*ppTextFormat)->SetLineSpacing(DWRITE_LINE_SPACING_METHOD_UNIFORM, fontSize * lineSpace, baseline);

	// 左右配置
	(*ppTextFormat)->SetTextAlignment(textAlignment);
	// 上下配置
	(*ppTextFormat)->SetParagraphAlignment(paragraphAlignment);
}

// 文字を描画
void Renderings::TextRenderHelper::DrawTextLayout
(
	float                   canvasRatio,
	const Math::Vector2&    position,
	ID2D1DeviceContext7*    pContext,
	IDWriteFactory8*        pDWriteFactory,
	IDWriteFontCollection3* pFontCollection,
	IDWriteTextLayout*      pTextLayout,
	ID2D1SolidColorBrush*   pBrush,
	DecorationTextRenderer* pDecorationRenderer,
	const Text*             pText
)
{
	// 色付け詳細リスト
	const auto& colorDescList = pText->GetDecorationString(m_refRenderer).GetColorDescList();
	// ルビ詳細リスト
	const auto& rubyDescList = pText->GetDecorationString(m_refRenderer).GetRubyDescList();

	// エフェクトを追加
	for (const auto& colorDesc : colorDescList)
	{
		pTextLayout->SetDrawingEffect
		(
			colorDesc.effect.Get(),
			DWRITE_TEXT_RANGE
			{
				static_cast<UINT32>(colorDesc.beginIndex),
				static_cast<UINT32>(colorDesc.length)
			}
		);
	}

	// 文字を描画
	pDecorationRenderer->Initialize(pContext, pBrush);
	pTextLayout->Draw
	(
		nullptr,
		pDecorationRenderer,
		position.x,
		position.y
	);

	// ルビを描画
	if (rubyDescList.size() != 0)
	{
		// フォントサイズ
		float fontSize = pText->GetFontSize() * canvasRatio;

		// テキストフォーマット
		Microsoft::WRL::ComPtr<IDWriteTextFormat> textFormat;
		CreateTextFormat
		(
			pText->GetFontName(),
			fontSize * 0.5f,
			1.0f,
			DWRITE_TEXT_ALIGNMENT_CENTER,
			DWRITE_PARAGRAPH_ALIGNMENT_CENTER,
			pDWriteFactory,
			pFontCollection,
			textFormat.GetAddressOf()
		);

		for (const auto& rubyDesc : rubyDescList)
		{
			// ヒット領域
			DWRITE_HIT_TEST_METRICS hitTestMetrics[8]{};
			// 領域数
			UINT32 hitTestMetricsCount = 0;

			pTextLayout->HitTestTextRange
			(
				static_cast<UINT32>(rubyDesc.beginIndex),
				static_cast<UINT32>(rubyDesc.length),
				position.x,
				position.y,
				hitTestMetrics,
				8,
				&hitTestMetricsCount
			);

			// ルビの長方形
			Math::Rect rubyRect
			{
				Math::Vector2{ hitTestMetrics[0].left, hitTestMetrics[0].top },
				Math::Vector2{ fontSize * rubyDesc.m_text.size(), fontSize }
			};

			// 文字の真上に描画位置が来るようにする
			switch (pText->GetParagraphAlignment())
			{
			case DWRITE_PARAGRAPH_ALIGNMENT_NEAR:
				rubyRect.position.y -= fontSize * 0.2f;
				break;
			case DWRITE_PARAGRAPH_ALIGNMENT_CENTER:
				rubyRect.position.y -= fontSize * (pText->GetLineSpace() / 2.0f - 0.8f);
				break;
			case DWRITE_PARAGRAPH_ALIGNMENT_FAR:
				rubyRect.position.y -= fontSize * (pText->GetLineSpace() - 1.4f);
				break;
			default:
				break;
			}

			// 各領域の幅を足す
			for (const auto& element : hitTestMetrics)
			{
				rubyRect.position.x += element.width / 2.0f;
			}

			// テキストレイアウト
			Microsoft::WRL::ComPtr<IDWriteTextLayout> textLayout;
			pDWriteFactory->CreateTextLayout
			(
				rubyDesc.m_text.c_str(),
				static_cast<UINT32>(rubyDesc.m_text.size()),
				textFormat.Get(),
				rubyRect.size.x,
				rubyRect.size.y,
				textLayout.GetAddressOf()
			);

			textLayout->Draw
			(
				nullptr,
				pDecorationRenderer,
				rubyRect.position.x - rubyRect.size.x / 2.0f,
				rubyRect.position.y - rubyRect.size.y / 2.0f
			);

		}
	}
}
