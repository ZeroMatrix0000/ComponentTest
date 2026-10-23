/*
 * FileName:     IGameObjectManager.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/23
 *
 * ゲームオブジェクト管理のインタフェース
 */

#pragma once

#include "IGameObjectFinder.h"
#include "IGameObjectInstantiator.h"

namespace GameObjects
{
	// ゲームオブジェクト管理のインタフェース
	class IGameObjectManager : public IGameObjectFinder, public IGameObjectInstantiator
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		IGameObjectManager()
			: IGameObjectFinder{}
			, IGameObjectInstantiator{}
		{
		}

		// 管理するゲームオブジェクトの参照を設定
		virtual void SetGameObjectsRef(Ref<std::vector<std::unique_ptr<GameObject>>> gameObjects) = 0;

		// ゲームオブジェクトを読み込む
		virtual void Load(const std::string& jsonName) = 0;

	};
}
