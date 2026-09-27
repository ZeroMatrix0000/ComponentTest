/*
 * FileName:     DeviceResources.h
 * Author:       Takao Hayata
 * Last Updated: 2026/07/06
 *
 * Direct3D 11に必要なリソース
 */

#pragma once

#include "Scripts/Commons/Systems/OnlyOne.h"

namespace Renderings
{
	// Direct3D 11に必要なリソース
	class DeviceResources : public Systems::OnlyOne
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		DeviceResources();

		// 初期化処理
		void Initialize(const HWND& hWindow);

		// 画面初期化
		void Clear();
		// 画面表示
		void Present();

		// ウィンドウサイズ変更時の処理
		void OnWindowSizeChanged(const Math::Vector2Int& outputSize);

		// デバイスの取得
		auto* GetD3DDevice()  const { return m_d3dDevice.Get(); }
		// デバイスコンテキストの取得
		auto* GetD3DContext() const { return m_d3dContext.Get(); }

		// スワップチェインの取得
		auto* GetSwapChain() const { return m_swapChain.Get(); }

		// DXGIデバイスを作成
		Microsoft::WRL::ComPtr<IDXGIDevice4> CreateDXGIDevice() const;


	private:


		/* メンバ関数 */

		// デバイスとコンテキストの作成
		void CreateDevice();
		// スワップチェインの作成
		void CreateSwapChain();
		// 描画ターゲットビューの作成
		void CreateRenderTargetView();
		// 深度ステンシルビューの作成
		void CreateDepthStencilView();


		/* メンバ変数 */

		// ウィンドウへのハンドル
		HWND             m_hWindow;
		// 出力サイズ
		Math::Vector2Int m_outputSize;

		// デバイス作成時の機能レベル
		D3D_FEATURE_LEVEL                            m_featureLevelSupported;
		// デバイス
		Microsoft::WRL::ComPtr<ID3D11Device5>        m_d3dDevice;
		// デバイスコンテキスト
		Microsoft::WRL::ComPtr<ID3D11DeviceContext4> m_d3dContext;

		// スワップチェイン
		Microsoft::WRL::ComPtr<IDXGISwapChain4> m_swapChain;

		// 描画ターゲットビュー
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;

		// ビューポートの設定
		D3D11_VIEWPORT                                 m_viewPort;
		// 深度ステンシルテクスチャ
		Microsoft::WRL::ComPtr<ID3D11Texture2D1>       m_depthStencil;
		// 深度ステンシルビュー
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;

	};
}
