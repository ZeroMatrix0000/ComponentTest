/*
 * FileName:     ComponentManager.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/23
 *
 * コンポーネント管理
 */

#pragma once

#include "IComponentManager.h"
#include "ComponentCreatePermit.h"

namespace GameObjects
{
	class GameObject;
	class IGameObjectInstantiator;
}

namespace Components
{

	// コンポーネント管理
	class ComponentManager : public IComponentManager
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		ComponentManager(Ref<IGameObjectInstantiator> iGameObjectInstantiator);

		// コンポーネント作成関数を追加
		template<typename TComponent> requires IsDerived<TComponent, Component>
		void RegisterCreate(const std::function<std::unique_ptr<Component>(const ComponentDesc&)>& CreateComponent)
		{
			m_CreateComponentList.emplace(typeid(TComponent*), CreateComponent);
		}


	private:


		/* メンバ関数 */

		// コンポーネントを作成
		std::unique_ptr<Component> Create(const std::type_index& index, Ref<GameObject> owner) override;

		// 未参照コンポーネントを取得
		Ref<Component> GetNullReference(const std::type_index& index, bool addErrorMessage) override;


		/* メンバ変数 */

		// コンポーネント作成許可証
		ComponentCreatePermit m_permit;

		// コンポーネント作成関数
		std::unordered_map<std::type_index, std::function<std::unique_ptr<Component>(const ComponentDesc&)>> m_CreateComponentList;

		// 未参照コンポーネントリスト
		std::unordered_map<std::type_index, std::unique_ptr<Component>> m_nullReferences;

		// ゲームオブジェクト生成インタフェースの参照
		Ref<IGameObjectInstantiator> m_refIGameObjectInstantiator;

	};
}
