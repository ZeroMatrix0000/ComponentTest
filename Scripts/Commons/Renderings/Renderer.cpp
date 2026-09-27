/*
 * FileName:     Renderer.h
 * Author:       Takao Hayata
 * Last Updated: 2026/09/28
 *
 * 描画
 */

#include "Pch.h"
#include "Renderer.h"

#include "Image.h"
#include "Text.h"

// コンストラクタ
Renderings::Renderer::Renderer()
	: OnlyOne{ typeid(Renderer) }
	, m_modelRenderer{}
	, m_imageRenderer{}
	, m_textRenderer{}
	, m_colliderRenderer{}
	, m_renderMode{}
{
}

// 初期化処理
void Renderings::Renderer::Initialize
(
	ID3D11Device5*               pDevice,
	ID3D11DeviceContext4*        pContext,
	IDXGISwapChain4*             pSwapChain,
	const DirectX::CommonStates& commonStates
)
{
	// モデル描画の初期化
	m_modelRenderer.Initialize(pContext, commonStates);
	// 画像描画の初期化
	m_imageRenderer.Initialize(pDevice, pContext, commonStates);
	// 当たり判定描画の初期化
	m_colliderRenderer.Initialize(pDevice, pContext, commonStates);
}

// 描画処理
void Renderings::Renderer::Render()
{
	// モデルの描画
	if (m_renderMode != RenderMode::Collider)
	{
		m_modelRenderer.Render();
	}
	// 当たり判定の描画
	if (m_renderMode != RenderMode::Model)
	{
		m_colliderRenderer.Render();
	}

	// 画像のポインタリストをソート
	m_imageRenderer.SortPImages();
	// 画像のポインタリスト
	const auto& pImages = m_imageRenderer.GetPImages();

	// テキストのポインタリストをソート
	m_textRenderer.SortPTexts();
	// テキストのポインタリスト
	const auto& pTexts = m_textRenderer.GetPTexts();

	// 画像イテレータ
	auto itImage = pImages.begin();
	// テキストイテレータ
	auto itText = pTexts.begin();

	// テキストを描画したか
	bool wasDrawText = false;

	// 描画開始
	m_imageRenderer.Begin();
	m_textRenderer.Begin();

	// どちらも描画し切るまで
	while (itImage != pImages.end() || itText != pTexts.end())
	{
		// 画像を描画し終わったなら
		if (itImage == pImages.end())
		{
			wasDrawText = true;
			m_textRenderer.Draw(*itText);
			itText++;
			continue;
		}
		// テキストを描画し終わったなら
		if (itText == pTexts.end())
		{
			// 一度テキストを描画したならリセット
			if (wasDrawText)
			{
				wasDrawText = false;
				m_imageRenderer.End();
				m_textRenderer.End();
				m_imageRenderer.Begin();
				m_textRenderer.Begin();
			}

			m_imageRenderer.Draw(*itImage);
			itImage++;
			continue;
		}

		// 画像の描画
		if ((*itImage)->GetOrderInLayer() <= (*itText)->GetOrderInLayer())
		{
			// 一度テキストを描画したならリセット
			if (wasDrawText)
			{
				wasDrawText = false;
				m_imageRenderer.End();
				m_textRenderer.End();
				m_imageRenderer.Begin();
				m_textRenderer.Begin();
			}

			m_imageRenderer.Draw(*itImage);
			itImage++;
			continue;
		}
		// テキストの描画
		else
		{
			wasDrawText = true;
			m_textRenderer.Draw(*itText);
			itText++;
			continue;
		}
	}

	// 描画終了
	m_imageRenderer.End();
	m_textRenderer.End();
}

// 描画モードを返る
void Renderings::Renderer::ChangeRenderMode()
{
	m_renderMode = static_cast<RenderMode>((static_cast<int>(m_renderMode) + 1) % static_cast<int>(RenderMode::Length));
}
