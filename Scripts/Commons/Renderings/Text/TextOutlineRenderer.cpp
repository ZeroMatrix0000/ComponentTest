/*
 * FileName:     TextOutLineRenderer.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/09/25
 *
 * テキストのアウトライン描画
 */

#include "Pch.h"
#include "TextOutlineRenderer.h"
#include "Text.h"
#include "../Components/RectTransform.h"

std::unordered_map<Renderings::TextOutlineRenderer::GlyphKey, Microsoft::WRL::ComPtr<ID2D1PathGeometry>, Renderings::TextOutlineRenderer::GlyphKeyHash> Renderings::TextOutlineRenderer::s_glyphCache{};

// コンストラクタ
Renderings::TextOutlineRenderer::TextOutlineRenderer(float canvasRatio, ID2D1Factory* pFactory, ID2D1RenderTarget* pRenderTarget, ID2D1SolidColorBrush* pTextBrush, ID2D1SolidColorBrush* pOutlineBrush, ID2D1StrokeStyle* pStrokeStyle, const Text& text)
	: m_geometry{}
	, m_sink{}
	, m_canvasRatio{ canvasRatio }
	, m_pFactory{ pFactory }
	, m_pRenderTarget{ pRenderTarget }
	, m_pTextBrush{ pTextBrush }
	, m_pOutlineBrush{ pOutlineBrush }
	, m_pStrokeStyle{ pStrokeStyle }
	, m_refText{ text }
{
}

// 描画
HRESULT __stdcall Renderings::TextOutlineRenderer::DrawGlyphRun(void* clientDrawingContext, FLOAT baselineOriginX, FLOAT baselineOriginY, DWRITE_MEASURING_MODE measuringMode, const DWRITE_GLYPH_RUN* glyphRun, const DWRITE_GLYPH_RUN_DESCRIPTION* glyphRunDescription, IUnknown* clientDrawingEffect)
{
	//// ジオメトリ
	//Microsoft::WRL::ComPtr<ID2D1PathGeometry> geometry;
	//HRESULT hr = m_pFactory->CreatePathGeometry(geometry.GetAddressOf());
	//if (FAILED(hr))
	//{
	//	return hr;
	//}

	//// 曲線
	//Microsoft::WRL::ComPtr<ID2D1GeometrySink> sink;
	//hr = geometry->Open(sink.GetAddressOf());
	//if (FAILED(hr))
	//{
	//	return hr;
	//}

	// アウトライン曲線を取得
	HRESULT hr = glyphRun->fontFace->GetGlyphRunOutline
	(
		glyphRun->fontEmSize,
		glyphRun->glyphIndices,
		glyphRun->glyphAdvances,
		glyphRun->glyphOffsets,
		glyphRun->glyphCount,
		glyphRun->isSideways,
		glyphRun->bidiLevel & 1,
		m_sink.Get()
	);
	if (FAILED(hr))
	{
		return hr;
	}

	//hr = sink->Close();
	//if (FAILED(hr))
	//{
	//	return hr;
	//}

	//// 古い座標系
	//D2D_MATRIX_3X2_F oldMatrix{};
	//m_pRenderTarget->GetTransform(&oldMatrix);

	//// 座標系を移動
	//m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(
	//	baselineOriginX,
	//	baselineOriginY
	//) * oldMatrix);

	//// アウトラインの描画
	//if (m_refText.GetOutlineWidth() != 0.0f)
	//{
	//	//m_pRenderTarget->DrawGeometry(geometry.Get(), m_pOutlineBrush, m_refText.GetOutlineWidth() * m_canvasRatio, m_pStrokeStyle);
	//}

	//m_pRenderTarget->FillGeometry(geometry.Get(), m_pTextBrush);

	//// 座標系を戻す
	//m_pRenderTarget->SetTransform(oldMatrix);

	return S_OK;
}

// 現在の座標系を取得
HRESULT Renderings::TextOutlineRenderer::GetCurrentTransform(void* clientDrawingContext, DWRITE_MATRIX* transform)
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
HRESULT Renderings::TextOutlineRenderer::GetPixelsPerDip(void* clientDrawingContext, FLOAT* pixelsPerDip)
{
	if (!pixelsPerDip)
	{
		return E_INVALIDARG;
	}

	*pixelsPerDip = 1.0f;
	return S_OK;
}

// 描画開始
void Renderings::TextOutlineRenderer::Begin()
{
	m_pFactory->CreatePathGeometry(m_geometry.ReleaseAndGetAddressOf());
	m_geometry->Open(m_sink.ReleaseAndGetAddressOf());
}

// 描画終了
void Renderings::TextOutlineRenderer::End()
{
	m_sink->Close();

	// 古い座標系
	D2D_MATRIX_3X2_F oldMatrix{};
	m_pRenderTarget->GetTransform(&oldMatrix);

	// 座標系を移動
	m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(
		100.0f,
		100.0f
	) * oldMatrix);

	// アウトラインの描画
	if (m_refText.GetOutlineWidth() != 0.0f)
	{
		m_pRenderTarget->DrawGeometry(m_geometry.Get(), m_pOutlineBrush, m_refText.GetOutlineWidth() * m_canvasRatio, m_pStrokeStyle);
	}

	m_pRenderTarget->FillGeometry(m_geometry.Get(), m_pTextBrush);

	// 座標系を戻す
	m_pRenderTarget->SetTransform(oldMatrix);
}

// ピクセルスナップ設定
HRESULT Renderings::TextOutlineRenderer::IsPixelSnappingDisabled(void* clientDrawingContext, BOOL* isDisabled)
{
	if (!isDisabled)
	{
		return E_INVALIDARG;
	}

	*isDisabled = true;
	return S_OK;
}

// グリフの取得
ID2D1PathGeometry* Renderings::TextOutlineRenderer::GetGlyphGeometry(IDWriteFontFace* fontFace, UINT16 glyphIndex, float fontSize, bool isSideways)
{
	// キー
	GlyphKey key{ fontFace, glyphIndex, fontSize, isSideways };

	auto it = s_glyphCache.find(key);
	if (it != s_glyphCache.end())
	{
		return it->second.Get();
	}

	// グリフ
	Microsoft::WRL::ComPtr<ID2D1PathGeometry> geometry;
	HRESULT hr = m_pFactory->CreatePathGeometry(geometry.GetAddressOf());
	if (FAILED(hr))
	{
		return nullptr;
	}

	// 曲線
	Microsoft::WRL::ComPtr<ID2D1GeometrySink> sink;
	hr = geometry->Open(sink.GetAddressOf());
	if (FAILED(hr))
	{
		return nullptr;
	}

	// アウトライン曲線を取得
	hr = fontFace->GetGlyphRunOutline
	(
		fontSize,
		&glyphIndex,
		nullptr,
		nullptr,
		1,
		isSideways,
		false,
		sink.Get()
	);
	if (FAILED(hr))
	{
		return nullptr;
	}

	hr = sink->Close();
	if (FAILED(hr))
	{
		return nullptr;
	}

	return s_glyphCache.emplace(key, std::move(geometry)).first->second.Get();
}

// グリフの描画
void Renderings::TextOutlineRenderer::DrawGlyphGeometry(ID2D1PathGeometry* pGeometry, const Math::Vector2& position)
{
	// 古い座標系
	D2D_MATRIX_3X2_F oldMatrix{};
	m_pRenderTarget->GetTransform(&oldMatrix);

	// 座標系を移動
	m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(
		position.x,
		position.y
	) * oldMatrix);

	// アウトラインの描画
	if (m_refText.GetOutlineWidth() != 0.0f)
	{
		m_pRenderTarget->DrawGeometry(pGeometry, m_pOutlineBrush, m_refText.GetOutlineWidth() * m_canvasRatio, m_pStrokeStyle);
	}

	m_pRenderTarget->FillGeometry(pGeometry, m_pTextBrush);

	// 座標系を戻す
	m_pRenderTarget->SetTransform(oldMatrix);
}
