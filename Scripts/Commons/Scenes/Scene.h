/*
 * FileName:     Scene.h
 * Author:       Takao Hayata
 * Last Updated: 2026/07/21
 *
 * シーン
 */

#pragma once

#include "../Components/Component.h"

namespace Scenes
{
	// シーン
	template<typename TTransitionData, typename TContext>
	class Scene : public Component
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		Scene(const ComponentDesc& desc)
			: Component{ desc }
			, m_gameObjects{}
			, m_pContext{}
		{
		}

		// 初期化処理
		void Initalize(const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder) override {};
		// 初期化処理
		virtual void Initialize(const TTransitionData& data) = 0;
		// 更新処理
		virtual void Update(float elapsedTime) = 0;
		// 終了処理
		virtual void Finalize() = 0;

		// コンテキストを設定
		void SetContext(const TContext& context) { m_pContext = &context; }


	protected:


		/* メンバ関数 */

		// ゲームオブジェクトリストを取得
		std::vector<std::unique_ptr<GameObject>>* GetPGameObjects() { return &m_gameObjects; }

		// コンテキストを取得
		const TContext& GetContext() const { return *m_pContext; }


	private:


		/* メンバ変数 */

		// ゲームオブジェクトリスト
		std::vector<std::unique_ptr<GameObject>> m_gameObjects;

		// コンテキストのポインタ
		const TContext* m_pContext;

	};
}
