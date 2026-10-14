/*
 * FileName:     EulerTargetCamera.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/14
 *
 * ターゲットカメラ（オイラー角）
 */

#pragma once

namespace Libraries
{
	namespace Camera
	{
		// ターゲットカメラ（オイラー角）
		struct EulerTargetCamera
		{

			/* メンバ関数 */

			// コンストラクタ
			constexpr EulerTargetCamera()
				: position{ Math::Vector3::Zero }
				, rotation{}
				, distance{ 1.0f }
			{
			}
			// コンストラクタ
			constexpr EulerTargetCamera(const Math::Vector3& position, const Math::Euler& rotation, float distance)
				: position{ position }
				, rotation{ rotation }
				, distance{ distance }
			{
			}

			// ビュー行列を作成
			Math::Matrix CreateViewMatrix() const;

			// 目線の位置を取得
			Math::Vector3 GetEyePosition() const;
			// 目線の方向を取得
			Math::Vector3 GetEyeDirection() const;


			/* 静的関数 */

			// 線形補間
			static EulerTargetCamera Lerp(const EulerTargetCamera& x, const EulerTargetCamera& y, float t)
			{
				return EulerTargetCamera
				{
					Math::Vector3::Lerp(x.position, y.position, t),
					Math::Euler::Lerp(x.rotation, y.rotation, t),
					Math::Lerp(x.distance, y.distance, t)
				};
			}


			/* メンバ変数 */

			// 位置
			Math::Vector3 position;
			// 回転
			Math::Euler   rotation;
			// ターゲットの距離
			float         distance;

		};
	}
}
