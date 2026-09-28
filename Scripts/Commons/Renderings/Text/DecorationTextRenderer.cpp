/*
 * FileName:     DecorationTextRenderer.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/09/28
 *
 * テキストのデコレーション描画
 */

#include "Pch.h"
#include "DecorationTextRenderer.h"
#include "Text.h"
#include "Scripts/Commons/Components/RectTransform.h"

// コンストラクタ
Renderings::DecorationTextRenderer::DecorationTextRenderer()
	: m_pContext{}
	, m_pBrush{}
{
}

// 描画
HRESULT __stdcall Renderings::DecorationTextRenderer::DrawGlyphRun(void* clientDrawingContext, FLOAT baselineOriginX, FLOAT baselineOriginY, DWRITE_MEASURING_MODE measuringMode, const DWRITE_GLYPH_RUN* glyphRun, const DWRITE_GLYPH_RUN_DESCRIPTION* glyphRunDescription, IUnknown* clientDrawingEffect)
{
	// 元の色
	D2D1_COLOR_F oldColor = m_pBrush->GetColor();

	// エフェクトを取得していたら
	if (clientDrawingEffect)
	{
		// 色付けエフェクト
		TextColorEffect* colorEffect = dynamic_cast<TextColorEffect*>(clientDrawingEffect);
		if (colorEffect)
		{
			m_pBrush->SetColor(colorEffect->GetColor());
		}
	}

	m_pContext->DrawGlyphRun
	(
		D2D1::Point2F(baselineOriginX, baselineOriginY),
		glyphRun,
		m_pBrush,
		measuringMode
	);
	m_pBrush->SetColor(oldColor);

	return S_OK;
}

// 現在の座標系を取得
HRESULT Renderings::DecorationTextRenderer::GetCurrentTransform(void* clientDrawingContext, DWRITE_MATRIX* transform)
{
	if (!transform)
	{
		return E_INVALIDARG;
	}

	transform->m11 = 1.0f;
	transform->m12 = 0.0f;
	transform->m21 = 0.0f;
	transform->m22 = 1.0f;
	transform->dx = 0.0f;
	transform->dy = 0.0f;

	return S_OK;
}

// DPI を取得
HRESULT Renderings::DecorationTextRenderer::GetPixelsPerDip(void* clientDrawingContext, FLOAT* pixelsPerDip)
{
	if (!pixelsPerDip)
	{
		return E_INVALIDARG;
	}

	*pixelsPerDip = 1.0f;
	return S_OK;
}

// 初期化処理
void Renderings::DecorationTextRenderer::Initialize(ID2D1DeviceContext7* pContext, ID2D1SolidColorBrush* pBrush)
{
	m_pContext = pContext;
	m_pBrush = pBrush;
}

// ピクセルスナップ設定
HRESULT Renderings::DecorationTextRenderer::IsPixelSnappingDisabled(void* clientDrawingContext, BOOL* isDisabled)
{
	if (!isDisabled)
	{
		return E_INVALIDARG;
	}

	*isDisabled = true;
	return S_OK;
}
