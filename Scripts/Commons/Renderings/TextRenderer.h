/*
 * FileName:     TextRenderer.h
 * Author:       Takao Hayata
 * Last Updated: 2026/09/28
 *
 * テキスト描画
 */

#pragma once

#include "ITextRenderer.h"
#include "TextOutlineRenderer.h"

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
		void Initialize(IDXGISwapChain4* pSwapChain, ID2D1DeviceContext7* pContext, ID2D1Bitmap1* pBackBuffer);
		// フォントコレクションの作成
		void CreateFontCollection(const std::wstring& directoryPath);

		// 描画開始
		void Begin();
		// 描画処理
		void Draw(const Text* pText);
		// 描画終了
		void End();

		// テキストのポインタを追加
		void AddPText(const Text* pText) override;
		// テキストのポインタを削除
		void RemovePText(const Text* pText) override;

		// テキストのポインタリストをソート
		void SortPTexts();

		// テキストのポインタリストを取得
		const std::vector<const Text*> GetPTexts() const { return m_pTexts; }


	private:


		/* メンバ変数 */

		// DirectWriteファクトリー
		Microsoft::WRL::ComPtr<IDWriteFactory8> m_dWriteFactory;

		// テキスト用ビットマップ
		Microsoft::WRL::ComPtr<ID2D1Bitmap1> m_textBitmap;

		// フォントコレクション
		Microsoft::WRL::ComPtr<IDWriteFontCollection3> m_fontCollection;

		// 線のスタイル
		Microsoft::WRL::ComPtr<ID2D1StrokeStyle> m_strokeStyle;

		// テキストのポインタリスト
		std::vector<const Text*> m_pTexts;

		// デバイスコンテキストのポインタ
		ID2D1DeviceContext7* m_pContext;
		// バックバッファのポインタ
		ID2D1Bitmap1* m_pBackBuffer;

	};
}
