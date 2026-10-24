/*
 * FileName:     Image.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/24
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
		Image(const ComponentDesc& desc, Ref<IImageRenderer> iImageRendererRef, Ref<Systems::IResources> iResources);
		// デストラクタ
		~Image();

		// 初期化処理
		void Initalize(const nlohmann::ordered_json& json, Ref<IGameObjectFinder> iGameObjectFinderRef) override;

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

		// キャンバスの参照を設定
		void SetCanvasRef(Ref<const Canvas> canvasRef) { m_canvasRef = canvasRef; }
		// 色を取得
		const Math::Color& GetColor() const { return m_color; }

		// レイヤー順を取得
		int GetOrderInLayer() const { return m_orderInLayer; }

		// 画像ソースを取得
		Ref<const ImageSource> GetImageSource() const { return m_imageSource; }
		// ピクセルシェーダを取得
		Ref<const PixelShader> GetPixelShader() const { return m_pixelShader; }

		// キャンバスの参照を取得
		Ref<const Canvas> GetCanvasRef() const { return m_canvasRef; }

		// 画像サイズを取得
		Math::Vector2 GetSize() const;


	private:


		/* メンバ変数 */

		// 色
		Math::Color m_color;

		// レイヤー順
		int m_orderInLayer;

		// 画像ソース
		Ref<const ImageSource> m_imageSource;
		// ピクセルシェーダ
		Ref<const PixelShader> m_pixelShader;

		// キャンバスの参照
		Ref<const Canvas> m_canvasRef;

		// 画像描画インタフェースの参照
		Ref<IImageRenderer> m_iImageRendererRef;

		// リソース管理の参照
		Ref<Systems::IResources> m_iResourcesRef;

	};
}
