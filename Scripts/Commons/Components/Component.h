/*
 * FileName:     Component.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/21
 *
 * コンポーネント
 */

#pragma once

#include "Observer.h"

namespace GameObjects
{
	class GameObject;
	class IGameObjectInstantiator;
	class IGameObjectFinder;
}

namespace Components
{
	struct ComponentDesc;

	// コンポーネント
	class Component : public Observer
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		Component(const ComponentDesc& desc);
		// デストラクタ
		virtual ~Component() = default;

		// TODO: 純粋仮想関数にする
		// 初期化処理
		virtual void Initalize(const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder) {};

		// 所有者を取得
		GameObject& GetOwner();
		// 変更不可所有者を取得
		const GameObject& GetConstOwner() const;


	protected:


		/* メンバ関数 */

		// ゲームオブジェクトを生成
		GameObject& Instantiate(const std::string& name) const;


	private:


		/* メンバ変数 */

		// 所有者のポインタ
		GameObject* m_pOwner;

		// ゲームオブジェクト生成インタフェース
		IGameObjectInstantiator* m_pIGameObjectInstantiator;

	};
}
