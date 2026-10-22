/*
 * FileName:     IGameObjectFinder.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/22
 *
 * ゲームオブジェクト検索のインタフェース
 */

#pragma once

#include "../Systems/OnlyOne.h"

namespace GameObjects
{
	class GameObject;

	// ゲームオブジェクト管理インタフェースのインタフェース
	class IGameObjectFinder : public Systems::OnlyOne
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		IGameObjectFinder()
			: OnlyOne{ typeid(IGameObjectFinder) }
		{
		}

		// ゲームオブジェクトを名前で検索
		virtual Ref<GameObject> Find(const std::string& name) const = 0;

	};
}
