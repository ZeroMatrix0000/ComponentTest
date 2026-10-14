/*
 * FileName:     GameObject.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/14
 *
 * ゲームオブジェクト
 */

#pragma once

#include "../Components/IComponentManager.h"

namespace Components
{
	class Component;
}

namespace GameObjects
{

	// ゲームオブジェクト
	class GameObject
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		GameObject(IComponentManager* pIComponentManager);

		// 名前を設定
		void SetName(const std::string& name) { m_name = name; }
		// 名前を取得
		const std::string& GetName() const { return m_name; }

		// メッセージを受け取る
		void AcceptMessage(const std::string& message) const;

		// コンポーネントを取得
		template<typename TComponent> requires IsDerived<TComponent, Component>
		TComponent* GetComponent()
		{
			auto it = m_pComponents.find(typeid(TComponent));
			if (it == m_pComponents.end())
			{
				return m_pIComponentManager->GetNullReferences<TComponent>(true);
			}
			return static_cast<TComponent*>(it->second);
		}
		// 変更不可コンポーネントを取得
		template<typename TComponent> requires IsDerived<TComponent, Component>
		const TComponent* GetConstComponent() const
		{
			auto it = m_pComponents.find(typeid(TComponent));
			if (it == m_pComponents.end())
			{
				return m_pIComponentManager->GetNullReferences<TComponent>(true);
			}
			return static_cast<TComponent*>(it->second);
		}

		// コンポーネントを追加
		template<typename TComponent> requires IsDerived<TComponent, Component>
		TComponent* AddComponent()
		{
			auto it = m_pComponents.find(typeid(TComponent));
			if (it != m_pComponents.end())
			{
				return static_cast<TComponent*>(it->second);
			}

			// コンポーネントを生成
			auto component = m_pIComponentManager->Create<TComponent>(this);
			auto pComponent = component.get();

			// コンポーネントを追加
			m_components.emplace(pComponent, std::move(component));
			// 取得用リストに追加
			AddPComponent<TComponent>(pComponent);

			return static_cast<TComponent*>(pComponent);
		}

		// 未参照コンポーネントを取得
		template<typename TComponent> requires IsDerived<TComponent, Component>
		TComponent* GetNullReferences()
		{
			return m_pIComponentManager->GetNullReferences<TComponent>();
		}


	private:


		/* メンバ関数 */

		// コンポーネントを取得用リストに追加
		template<typename TComponent> requires IsDerived<TComponent, Component>
		void AddPComponent(Component* pComponent)
		{
			m_pComponents.emplace(typeid(TComponent), pComponent);
			// 親コンポーネントが見つかったら
			if constexpr (requires{ typename TComponent::Base; })
			{
				using Base = TComponent::Base;

				AddPComponent<Base>(pComponent);
			}
		}


		/* メンバ変数 */

		// 名前
		std::string m_name;

		// コンポーネント
		std::unordered_map<Component*, std::unique_ptr<Component>> m_components;

		// コンポーネントの取得用
		std::unordered_map<std::type_index, Component*> m_pComponents;

		// コンポーネント管理のポインタ
		IComponentManager* m_pIComponentManager;

	};
}
