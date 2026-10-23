/*
 * FileName:     Image.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/23
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
		Image(const ComponentDesc& desc, IImageRenderer* pIImageRenderer, Systems::IResources* pIResources);
		// デストラクタ
		~Image();

		// 初期化処理
		void Initalize(const nlohmann::ordered_json& json, Ref<IGameObjectFinder> iGameObjectFinder) override;

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
		Ref<const ImageSource> GetImageSourceRef() const { return m_imageSourceRef; }
		// ピクセルシェーダ名を取得
		Ref<const PixelShader> GetPixelShaderRef() const { return m_pixelShaderRef; }

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

		// 画像ソースの参照
		Ref<const ImageSource> m_imageSourceRef;
		// ピクセルシェーダの参照
		Ref<const PixelShader> m_pixelShaderRef;

		// キャンバスのポインタ
		const Canvas* m_pCanvas;

		// 画像描画インタフェースのポインタ
		IImageRenderer* m_pIImageRenderer;

		// リソース管理のポインタ
		Systems::IResources* m_pIResources;

	};
}
