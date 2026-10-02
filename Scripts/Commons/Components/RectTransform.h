/*
 * FileName:     RectTransform.h
 * Author:       Takao Hayata
 * Last Updated: 2026/07/17
 *
 * 2D用トランスフォーム
 */

#pragma once

#include "Component.h"

namespace Components
{
	// 2D用トランスフォーム
	class RectTransform : public Component
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		RectTransform(const ComponentDesc& desc);

		// 初期化処理
		void Initalize(const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder) override;

		// 座標を設定
		void SetPosition(const Math::Vector2& position) { m_rect.position = position; }
		// 大きさを設定
		void SetSize(const Math::Vector2& size)         { m_rect.size = size; }
		// 角度を設定
		void SetAngle(float angle)                      { m_angle = angle; }

		// ピボットを設定
		void SetPivot(Utility::AlignmentPoint pivot)   { m_pivot = pivot; }
		// アンカーを設定
		void SetAnchor(Utility::AlignmentPoint anchor) { m_anchor = anchor; }

		// 長方形を取得
		const Math::Rect& GetRect()  const { return m_rect; }
		// 角度を取得
		float             GetAngle() const { return m_angle; }
		
		// ピボットを取得
		Utility::AlignmentPoint GetPivot()  const { return m_pivot; }
		// アンカーを取得
		Utility::AlignmentPoint GetAnchor() const { return m_anchor; }


	private:


		/* メンバ変数 */

		// 長方形
		Math::Rect m_rect;
		// 角度
		float m_angle;

		// ピボット
		Utility::AlignmentPoint m_pivot;
		// アンカー
		Utility::AlignmentPoint m_anchor;

	};
}