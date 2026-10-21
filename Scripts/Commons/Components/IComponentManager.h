/*
 * FileName:     IComponentManager.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/21
 *
 * コンポーネント管理のインタフェース
 */

#pragma once

#include "Component.h"
#include "../Systems/OnlyOne.h"

namespace Components
{
	struct ComponentDesc;

	// コンポーネント管理のインタフェース
	class IComponentManager : public Systems::OnlyOne
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		IComponentManager()
			: OnlyOne{ typeid(IComponentManager) }
		{
		}

		// コンポーネントを作成
		template<typename TComponent> requires IsDerived<TComponent, Component>
		std::unique_ptr<Component> Create(GameObject* pOwner = nullptr)
		{
			// コンポーネントのコンストラクタがDescのみなら作成関数を追加
			if constexpr (std::constructible_from<TComponent, const ComponentDesc&>)
			{
				RegisterCreate
				(
					typeid(TComponent),
					[](const ComponentDesc& desc)
					{
						return std::make_unique<TComponent>(desc);
					}
				);
			}
			return Create(typeid(TComponent), pOwner);
		};

		// 未参照コンポーネントを取得
		template<typename TComponent> requires IsDerived<TComponent, Component>
		TComponent& GetNullReferences(bool addErrorMessage = false)
		{
			// コンポーネントのコンストラクタがDescのみなら作成関数を追加
			if constexpr (std::constructible_from<TComponent, const ComponentDesc&>)
			{
				RegisterCreate
				(
					typeid(TComponent),
					[](const ComponentDesc& desc)
					{
						return std::make_unique<TComponent>(desc);
					}
				);
			}
			return static_cast<TComponent&>(GetNullReferences(typeid(TComponent), addErrorMessage));
		};


	private:


		/* メンバ関数 */

		// コンポーネントを作成
		virtual std::unique_ptr<Component> Create(const std::type_index& index, GameObject* pOwner) = 0;

		// コンポーネント作成関数を追加
		virtual void RegisterCreate
		(
			const std::type_index& index,
			const std::function<std::unique_ptr<Component>(const ComponentDesc&)>& CreateComponent
		) = 0;

		// 未参照コンポーネントを取得
		virtual Component& GetNullReferences(const std::type_index& index, bool addErrorMessage) = 0;


	};
}
