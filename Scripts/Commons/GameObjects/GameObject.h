/*
 * FileName:     GameObject.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/24
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
		GameObject(Ref<IComponentManager> iComponentManagerRef);

		// 名前を設定
		void SetName(const std::string& name) { m_name = name; }
		// 名前を取得
		const std::string& GetName() const { return m_name; }

		// メッセージを受け取る
		void AcceptMessage(const std::string& message) const;

		// コンポーネントを取得
		template<typename TComponent> requires IsDerived<TComponent, Component>
		Ref<TComponent> GetComponent()
		{
			auto it = m_componentRefs.find(typeid(Ref<TComponent>));
			if (it == m_componentRefs.end())
			{
				return m_iComponentManagerRef->GetNullComponent<TComponent>(true);
			}
			return it->second.ReinterpretCast<TComponent>();
		}
		// 変更不可コンポーネントを取得
		template<typename TComponent> requires IsDerived<TComponent, Component>
		Ref<const TComponent> GetConstComponent() const
		{
			auto it = m_componentRefs.find(typeid(Ref<TComponent>));
			if (it == m_componentRefs.end())
			{
				return m_iComponentManagerRef->GetNullComponent<TComponent>(true).Const();
			}
			return it->second.ReinterpretCast<TComponent>().Const();
		}

		// コンポーネントを追加
		template<typename TComponent> requires IsDerived<TComponent, Component>
		Ref<TComponent> AddComponent()
		{
			auto it = m_componentRefs.find(typeid(Ref<TComponent>));
			if (it != m_componentRefs.end())
			{
				return it->second.ReinterpretCast<TComponent>();
			}

			// コンポーネントを生成
			auto component = m_iComponentManagerRef->Create<TComponent>(Ref{ *this });
			Ref refComponent{ *component.get() };

			// コンポーネントを追加
			m_components.emplace(refComponent, std::move(component));
			// 取得用リストに追加
			AddComponentRef<TComponent>(refComponent);

			return refComponent.ReinterpretCast<TComponent>();
		}

		// 未参照コンポーネントを取得
		template<typename TComponent> requires IsDerived<TComponent, Component>
		Ref<TComponent> GetNullComponent() const
		{
			return m_iComponentManagerRef->GetNullComponent<TComponent>();
		}


	private:


		/* メンバ関数 */

		// コンポーネントを取得用リストに追加
		template<typename TComponent> requires IsDerived<TComponent, Component>
		void AddComponentRef(Ref<Component> component)
		{
			m_componentRefs.emplace(typeid(Ref<TComponent>), component);
			// 親コンポーネントが見つかったら
			if constexpr (requires{ typename TComponent::Base; })
			{
				using Base = TComponent::Base;

				AddComponentRef<Base>(component);
			}
		}


		/* メンバ変数 */

		// 名前
		std::string m_name;

		// コンポーネント
		std::unordered_map<Ref<Component>, std::unique_ptr<Component>> m_components;

		// コンポーネントの取得用
		std::unordered_map<std::type_index, Ref<Component>> m_componentRefs;

		// コンポーネント管理の参照
		Ref<IComponentManager> m_iComponentManagerRef;

	};
}
