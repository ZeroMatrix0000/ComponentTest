/*
 * FileName:     GameObjectManager.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/24
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
		GameObjectManager(Ref<IComponentManager> iComponentManagerRef, Ref<Systems::IResources> iResourcesRef);

		// 管理するゲームオブジェクトの参照を設定
		void SetGameObjectsRef(Ref<std::vector<std::unique_ptr<GameObject>>> gameObjects) override;

		// ゲームオブジェクトを読み込む
		void Load(const std::string& jsonName) override;

		// ゲームオブジェクトを名前で検索
		Ref<GameObject> Find(const std::string& name) const override;

		// ゲームオブジェクトを生成
		Ref<GameObject> Instantiate(const std::string& jsonName, bool dontDestroyOnLoad) override;

		// 未参照ゲームオブジェクトを取得
		Ref<GameObject> GetNullGameObject() const override { return Ref{ *m_nullReference }; }

		// コンポーネントを追加関数を追加
		template<typename TComponent> requires IsDerived<TComponent, Component>
		void Register(const std::string& componentName)
		{
			m_AddComponentList.emplace
			(
				componentName,
				[](Ref<GameObject> owner)
				{
					return owner->AddComponent<TComponent>().Cast<Component>();
				}
			);
		}


	private:


		/* メンバ関数 */

		// コンポーネントを追加
		std::vector<Component*> AddComponents(const nlohmann::ordered_json& json, Ref<GameObject> owner);
		// コンポーネントを初期化
		void InitializeComponents(const nlohmann::ordered_json& json, const std::vector<Component*>& pComponents);


		/* メンバ変数 */

		// コンポーネント追加関数
		std::unordered_map<std::string, std::function<Ref<Component>(Ref<GameObject>)>> m_AddComponentList;

		// 未参照ゲームオブジェクト
		std::unique_ptr<GameObject> m_nullReference;

		// シーンをまたいでも消えないゲームオブジェクトリスト
		std::vector<std::unique_ptr<GameObject>> m_dontDestroyOnLoadGameObjects;

		// 管理するゲームオブジェクトリストの参照
		Ref<std::vector<std::unique_ptr<GameObject>>> m_gameObjectRefs;

		// コンポーネント管理の参照
		Ref<IComponentManager> m_componentManagerRef;

		// リソースのインタフェースの参照
		Ref<Systems::IResources> m_iResourcesRef;

	};
}
