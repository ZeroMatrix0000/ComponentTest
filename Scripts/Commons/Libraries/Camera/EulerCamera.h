/*
 * FileName:     EulerCamera.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/14
 *
 * カメラ（オイラー角）
 */

#pragma once

namespace Libraries
{
	namespace Camera
	{
		// カメラ（オイラー角）
		struct EulerCamera
		{

			/* メンバ関数 */

			// コンストラクタ
			constexpr EulerCamera()
				: position{ Math::Vector3::Zero }
				, rotation{}
			{
			}
			// コンストラクタ
			constexpr EulerCamera(const Math::Vector3& position, const Math::Euler& rotation)
				: position{ position }
				, rotation{ rotation }
			{
			}

			// ビュー行列を作成
			Math::Matrix CreateViewMatrix() const;

			// 目線の位置を取得
			const Math::Vector3& GetEyePosition() const { return position; }
			// 目線の方向を取得
			Math::Vector3 GetEyeDirection() const;


			/* 静的関数 */

			// 線形補間
			static EulerCamera Lerp(const EulerCamera& x, const EulerCamera& y, float t)
			{
				return EulerCamera{ Math::Vector3::Lerp(x.position, y.position, t), Math::Euler::Lerp(x.rotation, y.rotation, t) };
			}


			/* メンバ変数 */

			// 位置
			Math::Vector3 position;
			// 回転
			Math::Euler   rotation;

		};
	}
}
