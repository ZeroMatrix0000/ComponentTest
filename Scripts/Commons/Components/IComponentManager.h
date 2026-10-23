/*
 * FileName:     IComponentManager.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/23
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
		std::unique_ptr<Component> Create(Ref<GameObject> owner)
		{
			return Create(typeid(TComponent*), owner);
		};

		// 未参照コンポーネントを取得
		template<typename TComponent> requires IsDerived<TComponent, Component>
		Ref<TComponent> GetNullReference(bool addErrorMessage = false)
		{
			return GetNullReference(typeid(TComponent*), addErrorMessage).ReinterpretCast<TComponent>();
		};


	private:


		/* メンバ関数 */

		// コンポーネントを作成
		virtual std::unique_ptr<Component> Create(const std::type_index& index, Ref<GameObject> owner) = 0;

		// 未参照コンポーネントを取得
		virtual Ref<Component> GetNullReference(const std::type_index& index, bool addErrorMessage) = 0;


	};
}
