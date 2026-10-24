/*
 * FileName:     ComponentDesc.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/24
 *
 * コンポーネント概要
 */

#pragma once

namespace GameObjects
{
	class GameObject;
	class IGameObjectInstantiator;
}

namespace Components
{
	class ComponentCreatePermit;

	// コンポーネント概要
	struct ComponentDesc
	{

		/* メンバ関数 */

		// コンストラクタ
		ComponentDesc(Ref<GameObject> owner, Ref<IGameObjectInstantiator> iGameObjectInstantiatorRef, const ComponentCreatePermit&)
			: owner{ owner }
			, iGameObjectInstantiatorRef{ iGameObjectInstantiatorRef }
		{
		}


		/* メンバ変数 */

		// 所有者の参照
		const Ref<GameObject> owner;
		// ゲームオブジェクト生成インタフェースの参照
		const Ref<IGameObjectInstantiator> iGameObjectInstantiatorRef;

	};
}
