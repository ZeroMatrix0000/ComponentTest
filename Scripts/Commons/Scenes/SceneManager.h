/*
 * FileName:     SceneManager.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/22
 *
 * シーン管理
 */

#pragma once

#include "ISceneManager.h"
#include "Scene.h"
#include "SceneTransitionAnimation.h"

namespace Scenes
{
	// シーン管理
	template<typename TTransitionData, typename TContext>
	class SceneManager : public ISceneManager<TTransitionData, TContext>
	{

		typedef Scene<TTransitionData, TContext> Scene;


	public:


		/* メンバ関数 */

		// コンストラクタ
		SceneManager(Ref<const TContext> refContext)
			: ISceneManager<TTransitionData, TContext>{}
			, m_currentScene{}
			, m_nextScene{}
			, m_CreateComponentList{}
			, m_data{}
			, m_animation{}
			, m_refContext{ refContext }
		{
		}

		// 初期化処理
		void Initialize(const Math::Vector2& outputSize, IGameObjectInstantiator* pIGameObjectInstantiator)
		{
			// アニメーションの初期化
			m_animation.Initialize(outputSize, pIGameObjectInstantiator);
		}
		// 更新処理
		void Update(float elapsedTime)
		{
			// アニメーションの更新
			m_animation.Update(elapsedTime);

			// 次のシーンが指定されていて、アニメーションが終わっているなら切り替える
			if (m_nextScene != nullptr && !m_animation.IsChanging())
			{
				// 現在のシーンの終了
				m_currentScene->Finalize();
				m_currentScene = std::move(m_nextScene);
				m_nextScene = nullptr;
				// 次のシーンの初期化
				m_currentScene->Initialize(m_data);

				// アニメーションの設定
				m_animation.SetAnimation(false);
			}

			// シーンの更新
			m_currentScene->Update(Math::Min(elapsedTime, 1.0f / 30.0f));
		}
		// 終了処理
		void Finalize()
		{
			// シーンの終了
			m_currentScene->Finalize();
		}

		// ウィンドウサイズ変更時の処理
		void OnWindowSizeChanged(const Math::Vector2& outputSize)
		{
			m_currentScene->AcceptMessage("WindowSizeChanged");
			m_animation.SetCanvasSize(outputSize);
		}

		// シーンを追加
		template<typename TScene> requires IsDerived<TScene, Scene>
		void AddScene(const std::function<std::unique_ptr<Scene>()>& CreateComponent)
		{
			m_CreateComponentList.emplace(typeid(TScene), CreateComponent);
		}

		// はじめのシーンを設定
		template<typename TScene> requires IsDerived<TScene, Scene>
		void SetFirstScene(const TTransitionData& data = TTransitionData{})
		{
			// シーンの初期化
			m_currentScene = m_CreateComponentList.at(typeid(TScene))();
			m_currentScene->SetContext(m_refContext);
			m_currentScene->Initialize(data);
		}

		// シーン切り替え中かどうか
		bool IsChanging() const override { return m_animation.IsChanging(); }


	private:


		/* メンバ関数 */

		// 次のシーンを設定
		void SetNextScene(const std::type_index& index, const TTransitionData& data) override
		{
			m_nextScene = m_CreateComponentList.at(index)();
			m_data = data;
			// アニメーションの設定
			m_animation.SetAnimation(true);
		}


		/* メンバ変数 */

		// 現在のシーン
		std::unique_ptr<Scene> m_currentScene;
		// 次のシーン
		std::unique_ptr<Scene> m_nextScene;

		// コンポーネント作成関数
		std::unordered_map<std::type_index, std::function<std::unique_ptr<Scene>()>> m_CreateComponentList;

		// シーン遷移データ
		TTransitionData m_data;

		// シーン切り替えアニメーション
		SceneTransitionAnimation m_animation;

		// コンテキストの参照
		Ref<const TContext> m_refContext;

	};
}
