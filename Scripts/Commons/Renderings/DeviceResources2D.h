/*
 * FileName:     DeviceResources2D.h
 * Author:       Takao Hayata
 * Last Updated: 2026/09/28
 *
 * Direct2D に必要なリソース
 */

#pragma once

#include "Scripts/Commons/Systems/OnlyOne.h"

namespace Renderings
{
	class DeviceResources;

	// Direct2D に必要なリソース
	class DeviceResources2D : public Systems::OnlyOne
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		DeviceResources2D(const DeviceResources& deviceResources);

		// 初期化処理
		void Initialize();

		// リセット
		void Reset();

		// ウィンドウサイズ変更時の処理
		void OnWindowSizeChanged();

		// ファクトリの取得
		auto* GetFactory()  const { return m_d2dFactory.Get(); }

		// コンテキストの取得
		auto* GetContext()  const { return m_d2dContext.Get(); }

		// 描画ターゲットの取得
		auto* GetBackBuffer()  const { return m_backBuffer.Get(); }


	private:


		/* メンバ関数 */

		// ファクトリの作成
		void CreateFactory();
		// デバイスの作成
		void CreateDevice();
		// バックバッファの作成
		void CreateBackBuffer();


		/* メンバ変数 */

		// Direct2Dファクトリー
		Microsoft::WRL::ComPtr<ID2D1Factory8> m_d2dFactory;

		// デバイス
		Microsoft::WRL::ComPtr<ID2D1Device7> m_d2dDevice;
		// デバイスコンテキスト
		Microsoft::WRL::ComPtr<ID2D1DeviceContext7> m_d2dContext;

		// バックバッファ
		Microsoft::WRL::ComPtr<ID2D1Bitmap1> m_backBuffer;

		// デバイスリソースの参照
		const DeviceResources& m_refDeviceResources;

	};
}
