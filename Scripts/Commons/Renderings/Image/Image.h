/*
 * FileName:     Image.h
 * Author:       Takao Hayata
 * Last Updated: 2026/09/29
 *
 * 画像
 */

#pragma once

#include "Scripts/Commons/Components/Component.h"

namespace Systems
{
	class IResources;
}

namespace Renderings
{
	class ImageSource;
	class PixelShader;
	class Canvas;
	class IImageRenderer;

	// 画像
	class Image : public Component
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		Image(const ComponentDesc& desc, IImageRenderer* pIImageRenderer, const Systems::IResources& iResources);
		// デストラクタ
		~Image();

		// 初期化処理
		void Initalize(const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder) override;

		// 色を設定
		void SetColor(const Math::Color& color) { m_color = color; }
		// 色を設定
		void SetColor(const DirectX::XMVECTORF32& color) { m_color = color; }

		// レイヤー順を設定
		void SetOrderInLayer(int orderInLayer) { m_orderInLayer = orderInLayer; }

		// 画像ソースを設定
		void SetImageSource(const std::string& imageSourceName);
		// ピクセルシェーダを設定
		void SetPixelShader(const std::string& pixelShaderName);

		// キャンバスを設定
		void SetCanvas(const Canvas& canvas) { m_pCanvas = &canvas; }
		// 色を取得
		const Math::Color& GetColor() const { return m_color; }

		// レイヤー順を取得
		int GetOrderInLayer() const { return m_orderInLayer; }

		// 画像ソース名を取得
		const Renderings::ImageSource* GetPImageSource() const { return m_pImageSource; }
		// ピクセルシェーダ名を取得
		const Renderings::PixelShader* GetPPixelShader() const { return m_pPixelShader; }

		// キャンバスのポインタを取得
		const Canvas* GetPCanvas() const { return m_pCanvas; }

		// 画像サイズを取得
		Math::Vector2 GetSize() const;


	private:


		/* メンバ変数 */

		// 色
		Math::Color m_color;

		// レイヤー順
		int m_orderInLayer;

		// 画像ソース
		const Renderings::ImageSource* m_pImageSource;
		// ピクセルシェーダ
		const Renderings::PixelShader* m_pPixelShader;

		// キャンバスのポインタ
		const Canvas* m_pCanvas;

		// 画像描画インタフェースのポインタ
		IImageRenderer* m_pIImageRenderer;

		// リソース管理
		const Systems::IResources& m_refIResources;

	};
}
