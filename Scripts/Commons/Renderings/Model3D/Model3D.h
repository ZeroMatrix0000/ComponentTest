/*
 * FileName:     Model3D.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/07
 *
 * 3Dモデル
 */

#pragma once

#include "Scripts/Commons/Components/Component.h"

namespace Systems
{
	class IResources;
}

namespace Renderings
{
	class Model3DSource;
	class ICameraScreen;
	class IModel3DRenderer;

	// 3Dモデル
	class Model3D : public Component
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		Model3D(const ComponentDesc& desc, IModel3DRenderer* pIModelRenderer, const Systems::IResources& iResources);
		// デストラクタ
		~Model3D();

		// 初期化処理
		void Initalize(const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder) override;

		// モデルソースを設定
		void SetModelSource(const std::string& modelSourceName);

		// 映るカメラ画面を追加
		void AddICameraScreen(const ICameraScreen& iCameraScreen);
		// 映るカメラ画面を削除
		void RemoveICameraScreen(const ICameraScreen& iCameraScreen);

		// モデルソースを取得
		const Model3DSource* GetPModelSource() const { return m_pModelSource; }


	private:


		/* メンバ変数 */

		// モデルソース
		const Model3DSource* m_pModelSource;

		// 映るカメラ画面のポインタリスト
		std::vector<const ICameraScreen*> m_pICameraScreens;

		// モデル描画インタフェースのポインタ
		IModel3DRenderer* m_pIModelRenderer;

		// リソース管理
		const Systems::IResources& m_refIResources;

	};
}
