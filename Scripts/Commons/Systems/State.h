/*
 * FileName:     State.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/14
 *
 * 状態
 */

#pragma once

namespace Systems
{
	// 状態
	template<typename TInternals>
	class State
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		State() = default;
		// デストラクタ
		virtual ~State() = default;

		// 開始処理
		virtual void Enter(TInternals* pInternals) = 0;
		// 更新処理
		virtual void Update(TInternals* pInternals, float elapsedTime) = 0;
		// 終了処理
		virtual void Exit(TInternals* pInternals) = 0;

		// 次のシーンをリセット
		void ResetNextState() { m_nextState.reset(); }

		// 次のシーンをムーブ
		std::unique_ptr<State> MoveNextState() { return std::move(m_nextState); }

		// 次のシーンを取得
		State* GetNextState() const { return m_nextState.get(); }


	protected:


		/* メンバ関数 */

		// 次のシーンを設定
		void SetNextState(std::unique_ptr<State> nextState) { m_nextState = std::move(nextState); }


	private:


		/* メンバ変数 */

		// 次の状態
		std::unique_ptr<State> m_nextState;

	};
}
