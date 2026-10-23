/*
 * FileName:     IResources.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/23
 *
 * リソースのインタフェース
 */

#pragma once

#include "OnlyOne.h"

namespace Renderings
{
	class Model3DSource;
	class ImageSource;
	class VertexShader;
	class PixelShader;
}

namespace Systems
{
	// リソースのインタフェース
	class IResources : public OnlyOne
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		IResources()
			: OnlyOne{ typeid(IResources) }
		{
		}
		// デストラクタ
		virtual ~IResources() = default;

		// モデルの取得
		virtual Ref<const Renderings::Model3DSource> GetModelSource(const std::string& modelName) = 0;
		// 画像の取得
		virtual Ref<const Renderings::ImageSource>   GetImageSource(const std::string& imageName) = 0;
		// Jsonの取得
		virtual Ref<const nlohmann::ordered_json>    GetJson(const std::string& jsonName) = 0;
		// メッシュの取得
		virtual Ref<const Mesh>                      GetMesh(const std::string& meshName) = 0;
		// 頂点シェーダの取得
		virtual Ref<const Renderings::VertexShader>  GetVertexShader(const std::string& shaderName) = 0;
		// ピクセルシェーダの取得
		virtual Ref<const Renderings::PixelShader>   GetPixelShader(const std::string& shaderName) = 0;

		// モデルの取得
		virtual Ref<const Renderings::Model3DSource> GetNullModelSource() = 0;
		// 画像の取得
		virtual Ref<const Renderings::ImageSource>   GetNullImageSource() = 0;
		// Jsonの取得
		virtual Ref<const nlohmann::ordered_json>    GetNullJson() = 0;
		// メッシュの取得
		virtual Ref<const Mesh>                      GetNullMesh() = 0;
		// 頂点シェーダの取得
		virtual Ref<const Renderings::VertexShader>  GetNullVertexShader() = 0;
		// ピクセルシェーダの取得
		virtual Ref<const Renderings::PixelShader>   GetNullPixelShader()  = 0;

	};
}
