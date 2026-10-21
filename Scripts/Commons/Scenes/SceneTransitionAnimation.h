/*
 * FileName:     SceneTransitionAnimation.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/21
 *
 * シーン遷移時のアニメーション
 */

#pragma once

namespace GameObjects
{
	class GameObject;
	class IGameObjectInstantiator;
}
namespace Components
{
	class RectTransform;
}
namespace Renderings
{
	class Canvas;
}

namespace Scenes
{
	// シーン遷移時のアニメーション
	class SceneTransitionAnimation
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		SceneTransitionAnimation();

		// 初期化処理
		void Initialize(const Math::Vector2& outputSize, IGameObjectInstantiator* pIGameObjectInstantiator);
		// 更新処理
		void Update(float elapsedTime);

		// キャンバスサイズの変更
		void SetCanvasSize(const Math::Vector2& outputSize);

		// 切り替え率を設定
		void SetAnimation(bool isEnter);

		// 切り替え中かどうか
		bool IsChanging() const { return m_changingRate.IsMoving(); }


	private:


		/* 定数 */

		// 切り替えにかける時間
		static constexpr float CHANGE_TIME = 0.25f;


		/* メンバ変数 */

		// 角度
		float m_angle;

		// 切り替え率
		Easing::Value<float> m_changingRate;

		// 画像の2D用トランスフォームのポインタ
		RectTransform* m_pImageRectTransform;

		// キャンバスのポインタ
		Renderings::Canvas* m_pCanvas;

	};
}
