/*
 * FileName:     GameObject.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/23
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
		GameObject(Ref<IComponentManager> refIComponentManager);

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
			auto it = m_refComponents.find(typeid(Ref<TComponent>));
			if (it == m_refComponents.end())
			{
				return m_refIComponentManager->GetNullReference<TComponent>(true);
			}
			return it->second.ReinterpretCast<TComponent>();
		}
		// 変更不可コンポーネントを取得
		template<typename TComponent> requires IsDerived<TComponent, Component>
		Ref<const TComponent> GetConstComponent() const
		{
			auto it = m_refComponents.find(typeid(Ref<TComponent>));
			if (it == m_refComponents.end())
			{
				return m_refIComponentManager->GetNullReference<TComponent>(true).Const();
			}
			return it->second.ReinterpretCast<TComponent>().Const();
		}

		// コンポーネントを追加
		template<typename TComponent> requires IsDerived<TComponent, Component>
		Ref<TComponent> AddComponent()
		{
			auto it = m_refComponents.find(typeid(Ref<TComponent>));
			if (it != m_refComponents.end())
			{
				return it->second.ReinterpretCast<TComponent>();
			}

			// コンポーネントを生成
			auto component = m_refIComponentManager->Create<TComponent>(Ref{ *this });
			Ref refComponent{ *component.get() };

			// コンポーネントを追加
			m_components.emplace(refComponent, std::move(component));
			// 取得用リストに追加
			AddRefComponent<TComponent>(refComponent);

			return refComponent.ReinterpretCast<TComponent>();
		}

		// 未参照コンポーネントを取得
		template<typename TComponent> requires IsDerived<TComponent, Component>
		Ref<TComponent> GetNullReference()
		{
			return m_refIComponentManager->GetNullReference<TComponent>();
		}


	private:


		/* メンバ関数 */

		// コンポーネントを取得用リストに追加
		template<typename TComponent> requires IsDerived<TComponent, Component>
		void AddRefComponent(Ref<Component> component)
		{
			m_refComponents.emplace(typeid(Ref<TComponent>), component);
			// 親コンポーネントが見つかったら
			if constexpr (requires{ typename TComponent::Base; })
			{
				using Base = TComponent::Base;

				AddRefComponent<Base>(component);
			}
		}


		/* メンバ変数 */

		// 名前
		std::string m_name;

		// コンポーネント
		std::unordered_map<Ref<Component>, std::unique_ptr<Component>> m_components;

		// コンポーネントの取得用
		std::unordered_map<std::type_index, Ref<Component>> m_refComponents;

		// コンポーネント管理のポインタ
		Ref<IComponentManager> m_refIComponentManager;

	};
}
