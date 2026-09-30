/*
 * FileName:     TextRenderer.h
 * Author:       Takao Hayata
 * Last Updated: 2026/09/30
 *
 * テキスト描画
 */

#pragma once

#include "ITextRenderer.h"
#include "DecorationTextRenderer.h"

namespace Renderings
{
	class PixelShader;
	
	// テキスト描画
	class TextRenderer : public ITextRenderer
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		TextRenderer();

		// 初期化処理
		void Initialize(ID2D1Device7* pDevice, ID2D1DeviceContext7* pContext, ID2D1Bitmap1* pBackBuffer);
		// フォントコレクションの作成
		void CreateFontCollection(const std::wstring& directoryPath);

		// 描画開始
		void Begin();
		// 描画処理
		void Draw(const Text* pText, bool isDebug);
		// 描画終了
		void End();

		// ウィンドウサイズ変更時の処理
		void OnWindowSizeChanged(ID2D1DeviceContext7* pContext, ID2D1Bitmap1* pBackBuffer);

		// テキストのポインタを追加
		void AddPText(const Text* pText) override;
		// テキストのポインタを削除
		void RemovePText(const Text* pText) override;

		// テキストのポインタリストをソート
		void SortPTexts();

		// テキストのポインタリストを取得
		const std::vector<const Text*> GetPTexts() const { return m_pTexts; }


	private:


		/* メンバ関数 */

		// 文字を描画
		void DrawTextLayout
		(
			const Math::Vector2&  position,
			ID2D1DeviceContext7*  pContext,
			IDWriteTextLayout*    pTextLayout,
			ID2D1SolidColorBrush* pBrush,
			const Text*           pText
		);


		/* メンバ変数 */

		// DirectWriteファクトリー
		Microsoft::WRL::ComPtr<IDWriteFactory8> m_dWriteFactory;

		// テキスト用コンテキスト
		Microsoft::WRL::ComPtr<ID2D1DeviceContext7> m_textContext;

		// フォントコレクション
		Microsoft::WRL::ComPtr<IDWriteFontCollection3> m_fontCollection;

		// 膨張エフェクト
		Microsoft::WRL::ComPtr<ID2D1Effect> m_dilateEffect;
		// 色変更エフェクト
		Microsoft::WRL::ComPtr<ID2D1Effect> m_floodEffect;
		// 画像合体エフェクト
		Microsoft::WRL::ComPtr<ID2D1Effect> m_compositeEffect;

		// デコレーション描画
		DecorationTextRenderer m_decorationRenderer;

		// テキストのポインタリスト
		std::vector<const Text*> m_pTexts;

		// デバイスコンテキストのポインタ
		ID2D1DeviceContext7* m_pContext;
		// バックバッファのポインタ
		ID2D1Bitmap1* m_pBackBuffer;

	};
}
