/*
 * FileName:     GameObjectManager.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/21
 *
 * ゲームオブジェクト管理
 */

#pragma once

#include "IGameObjectManager.h"
#include "GameObject.h"

namespace Systems
{
	class IResources;
}

namespace GameObjects
{
	// ゲームオブジェクト管理
	class GameObjectManager : public IGameObjectManager
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		GameObjectManager(Systems::IResources* pIResources);

		// 初期化処理
		void Initialize(IComponentManager* pIComponentManager);

		// 管理するゲームオブジェクトのポインタを設定
		void SetPGameObjects(std::vector<std::unique_ptr<GameObject>>* pGameObjects) override;

		// ゲームオブジェクトを読み込む
		void Load(const std::string& jsonName) override;

		// ゲームオブジェクトを名前で検索
		GameObject& Find(const std::string& name) const override;

		// ゲームオブジェクトを生成
		GameObject& Instantiate(const std::string& jsonName, bool dontDestroyOnLoad) override;

		// 未参照ゲームオブジェクトを取得
		GameObject& GetNullReferences() const override { return *m_nullReference; }

		// コンポーネントを追加関数を追加
		template<typename TComponent> requires IsDerived<TComponent, Component>
		void Register(const std::string& componentName)
		{
			m_AddComponentList.emplace
			(
				componentName,
				[](GameObject* pOwner)
				{
					return &pOwner->AddComponent<TComponent>();
				}
			);
		}


	private:


		/* メンバ関数 */

		// コンポーネントを追加
		std::vector<Component*> AddComponents(const nlohmann::ordered_json& json, GameObject* pGameObject);
		// コンポーネントを初期化
		void InitializeComponents(const nlohmann::ordered_json& json, const std::vector<Component*>& pComponents);


		/* メンバ変数 */

		// コンポーネント追加関数
		std::unordered_map<std::string, std::function<Component*(GameObject*)>> m_AddComponentList;

		// 未参照ゲームオブジェクト
		std::unique_ptr<GameObject> m_nullReference;

		// 管理するゲームオブジェクトリストのポインタ
		std::vector<std::unique_ptr<GameObject>>* m_pGameObjects;

		// シーンをまたいでも消えないゲームオブジェクトリスト
		std::vector<std::unique_ptr<GameObject>> m_dontDestroyOnLoadGameObjects;

		// コンポーネント管理のポインタ
		IComponentManager* m_pIComponentManager;

		// リソースのインタフェースのポインタ
		Systems::IResources* m_pIResources;

	};
}
