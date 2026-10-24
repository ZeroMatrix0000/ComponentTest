/*
 * FileName:     IGameObjectInstantiator.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/24
 *
 * ゲームオブジェクト生成のインタフェース
 */

#pragma once

#include "../Systems/OnlyOne.h"

namespace GameObjects
{
	class GameObject;

	// ゲームオブジェクト管理のインタフェース
	class IGameObjectInstantiator : public Systems::OnlyOne
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		IGameObjectInstantiator()
			: OnlyOne{ typeid(IGameObjectInstantiator) }
		{
		}

		// ゲームオブジェクトを生成
		virtual Ref<GameObject> Instantiate(const std::string& jsonName, bool dontDestroyOnLoad = false) = 0;

		// 未参照ゲームオブジェクトを取得
		virtual Ref<GameObject> GetNullGameObject() const = 0;

	};
}
