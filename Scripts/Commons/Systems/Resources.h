/*
 * FileName:     Resources.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/07
 *
 * リソース管理
 */

#pragma once

#include "IResources.h"
#include "../Renderings/Model3D/Model3DSource.h"
#include "../Renderings/Image/ImageSource.h"
#include "../Renderings/VertexShader.h"
#include "../Renderings/PixelShader.h"

namespace Systems
{
	// リソース管理
	class Resources : public IResources
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		Resources();

		// モデルを読み込む
		void LoadModelSources(ID3D11Device5* device, DirectX::IEffectFactory* fx, const std::wstring& directoryPath);
		// 画像を読み込む
		void LoadImageSources(ID3D11Device5* device, const std::wstring& directoryPath);
		// Jsonを読み込む
		void LoadJsons(const std::wstring& directoryPath);
		// Jsonを文字列から追加
		void AddJsonFromStr(const std::string& jsonName, const std::string& str);
		// メッシュを読み込む
		void LoadMeshes(const std::wstring& directoryPath);
		// 頂点シェーダを読み込む
		void LoadVertexShaders(ID3D11Device5* device, const std::wstring& directoryPath);
		// ピクセルシェーダを読み込む
		void LoadPixelShaders(ID3D11Device5* device, const std::wstring& directoryPath);

		// モデルの取得
		const Renderings::Model3DSource* GetModelSource(const std::string& modelName)   const override;
		// 画像の取得
		const Renderings::ImageSource*   GetImageSource(const std::string& imageName)   const override;
		// Jsonの取得
		const nlohmann::ordered_json*    GetJson(const std::string& jsonName)           const override;
		// Jsonの取得
		const Mesh*                      GetMesh(const std::string& meshName)           const override;
		// 頂点シェーダの取得
		const Renderings::VertexShader*  GetVertexShader(const std::string& shaderName) const override;
		// ピクセルシェーダの取得
		const Renderings::PixelShader*   GetPixelShader(const std::string& shaderName)  const override;


	private:

		/* メンバ関数 */


		/* メンバ変数 */

		// モデルソースリスト
		std::unordered_map<std::string, Renderings::Model3DSource> m_modelSources;
		// 画像ソースリスト
		std::unordered_map<std::string, Renderings::ImageSource> m_imageSources;
		// Jsonリスト
		std::unordered_map<std::string, nlohmann::ordered_json> m_jsons;
		// メッシュリスト
		std::unordered_map<std::string, Mesh> m_meshes;
		// 頂点シェーダリスト
		std::unordered_map<std::string, Renderings::VertexShader> m_vertexShaders;
		// ピクセルシェーダリスト
		std::unordered_map<std::string, Renderings::PixelShader> m_pixelShaders;

	};
}
