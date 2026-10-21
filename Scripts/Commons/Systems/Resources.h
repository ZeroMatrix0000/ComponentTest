/*
 * FileName:     Resources.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/21
 *
 * リソース管理
 */

#pragma once

#include "IResources.h"
#include "../Renderings/Model3D/Model3DSource.h"
#include "../Renderings/Image/ImageSource.h"
#include "../Renderings/Shader/VertexShader.h"
#include "../Renderings/Shader/PixelShader.h"

namespace Renderings
{
	class InstancingEffectFactory;
}

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
		void LoadModelSources(ID3D11Device5* device, Renderings::InstancingEffectFactory* fx, const std::wstring& directoryPath);
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
		const Renderings::Model3DSource* GetModelSource(const std::string& modelName)   override;
		// 画像の取得
		const Renderings::ImageSource*   GetImageSource(const std::string& imageName)   override;
		// Jsonの取得
		const nlohmann::ordered_json*    GetJson(const std::string& jsonName)           override;
		// メッシュの取得
		const Mesh*                      GetMesh(const std::string& meshName)           override;
		// 頂点シェーダの取得
		const Renderings::VertexShader*  GetVertexShader(const std::string& shaderName) override;
		// ピクセルシェーダの取得
		const Renderings::PixelShader*   GetPixelShader(const std::string& shaderName)  override;

		// モデルの取得
		const Renderings::Model3DSource* GetNullModelSource()  override { return &m_nullModelSource; }
		// 画像の取得
		const Renderings::ImageSource*   GetNullImageSource()  override { return &m_nullImageSource; }
		// Jsonの取得
		const nlohmann::ordered_json*    GetNullJson()         override { return &m_nullJson; };
		// メッシュの取得
		const Mesh*                      GetNullMesh()         override { return &m_nullMesh; }
		// 頂点シェーダの取得
		const Renderings::VertexShader*  GetNullVertexShader() override { return &m_nullVertexShader; }
		// ピクセルシェーダの取得
		const Renderings::PixelShader*   GetNullPixelShader()  override { return &m_nullPixelShader; }
		

		// モデルの取得
		Renderings::Model3DSource* GetEditableModelSource(const std::string& modelName);
		// 画像の取得
		Renderings::ImageSource*   GetEditableImageSource(const std::string& imageName);
		// Jsonの取得
		nlohmann::ordered_json*    GetEditableJson(const std::string& jsonName);
		// メッシュの取得
		Mesh*                      GetEditableMesh(const std::string& meshName);
		// 頂点シェーダの取得
		Renderings::VertexShader*  GetEditableVertexShader(const std::string& shaderName);
		// ピクセルシェーダの取得
		Renderings::PixelShader*   GetEditablePixelShader(const std::string& shaderName);


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

		// モデルソース
		Renderings::Model3DSource m_nullModelSource;
		// 画像ソース
		Renderings::ImageSource m_nullImageSource;
		// Json
		nlohmann::ordered_json m_nullJson;
		// メッシュ
		Mesh m_nullMesh;
		// 頂点シェーダ
		Renderings::VertexShader m_nullVertexShader;
		// ピクセルシェーダ
		Renderings::PixelShader m_nullPixelShader;

	};
}
