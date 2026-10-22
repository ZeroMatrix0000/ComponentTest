/*
 * FileName:     ComponentDesc.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/22
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
		ComponentDesc(Ref<GameObject> owner, Ref<IGameObjectInstantiator> iGameObjectInstantiator, Ref<const ComponentCreatePermit>)
			: refOwner{ owner }
			, refIGameObjectInstantiator{ iGameObjectInstantiator }
		{
		}


		/* メンバ変数 */

		// 所有者のポインタ
		const Ref<GameObject> refOwner;
		// ゲームオブジェクト生成インタフェース
		const Ref<IGameObjectInstantiator> refIGameObjectInstantiator;

	};
}
