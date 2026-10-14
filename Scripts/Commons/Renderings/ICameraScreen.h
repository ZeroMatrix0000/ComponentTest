/*
 * FileName:     ICameraScreen.h
 * Author:       Takao Hayata
 * Last Updated: 2026/09/10
 *
 * カメラ画面のインタフェース
 */

#pragma once

#include "../Components/Component.h"

namespace Renderings
{
	// カメラ画面のインタフェース
	class ICameraScreen : public Component
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		ICameraScreen(const ComponentDesc& desc)
			: Component{ desc }
		{
		}

		// ビュー行列を更新
		virtual void UpdateViewMatrix() = 0;

		// 座標を設定
		virtual void SetPosition(const Math::Vector3& position) = 0;

		// プロジェクション行列を設定
		virtual void SetProjectionMatrix(const Math::Vector2& outputSize) = 0;

		// ビュー行列を取得
		virtual const Math::Matrix& GetViewMatrix() const = 0;
		// プロジェクション行列を取得
		virtual const Math::Matrix& GetProjectionMatrix() const = 0;

		// 目の座標を取得
		virtual const Math::Vector3 GetEyePosition() const = 0;
		// 回転を取得
		virtual const Math::Vector3 GetEyeDirection() const = 0;

		// 出力サイズを取得
		virtual const Math::Vector2& GetOutputSize() const = 0;

	};
}
