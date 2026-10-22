/*
 * FileName:     Scene.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/22
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
		{
		}

		// 初期化処理
		void Initalize(const nlohmann::ordered_json& json, Ref<IGameObjectFinder> iGameObjectFinder) override {};
		// 初期化処理
		virtual void Initialize(const TTransitionData& data) = 0;
		// 更新処理
		virtual void Update(float elapsedTime) = 0;
		// 終了処理
		virtual void Finalize() = 0;


		/* 静的関数 */

		// コンテキストを設定
		static void SetContext(Ref<const TContext> context) { s_pContext = &context.Get(); }


	protected:


		/* 静的関数 */

		// コンテキストを取得
		static Ref<const TContext> GetContext() { return Ref{ *s_pContext }; }


		/* メンバ関数 */

		// ゲームオブジェクトリストを取得
		Ref<std::vector<std::unique_ptr<GameObject>>> GetGameObjects() { return Ref{ m_gameObjects }; }


	private:


		/* 静的変数 */

		// コンテキストのポインタ
		inline static const TContext* s_pContext{ nullptr };


		/* メンバ変数 */

		// ゲームオブジェクトリスト
		std::vector<std::unique_ptr<GameObject>> m_gameObjects;

	};
}
