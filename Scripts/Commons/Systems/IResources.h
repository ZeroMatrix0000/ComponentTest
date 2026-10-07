/*
 * FileName:     IResources.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/07
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
		virtual const Renderings::Model3DSource* GetModelSource(const std::string& modelName)   const = 0;
		// 画像の取得
		virtual const Renderings::ImageSource*   GetImageSource(const std::string& imageName)   const = 0;
		// Jsonの取得
		virtual const nlohmann::ordered_json*    GetJson(const std::string& jsonName)           const = 0;
		// メッシュの取得
		virtual const Mesh*                      GetMesh(const std::string& meshName)           const = 0;
		// 頂点シェーダの取得
		virtual const Renderings::VertexShader*  GetVertexShader(const std::string& shaderName) const = 0;
		// ピクセルシェーダの取得
		virtual const Renderings::PixelShader*   GetPixelShader(const std::string& shaderName)  const = 0;

	};
}
