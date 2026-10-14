/*
 * FileName:     QuaternionTargetCamera.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/14
 *
 * ターゲットカメラ（四元数）
 */

#pragma once

namespace Libraries
{
	namespace Camera
	{
		// ターゲットカメラ（四元数）
		struct QuaternionTargetCamera
		{

			/* メンバ関数 */

			// コンストラクタ
			constexpr QuaternionTargetCamera()
				: position{ Math::Vector3::Zero }
				, rotation{ Math::Quaternion::Identity }
				, distance{ 1.0f }
			{
			}
			// コンストラクタ
			constexpr QuaternionTargetCamera(const Math::Vector3& position, const Math::Quaternion& rotation, float distance)
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
			static QuaternionTargetCamera Lerp(const QuaternionTargetCamera& x, const QuaternionTargetCamera& y, float t)
			{
				return QuaternionTargetCamera
				{
					Math::Vector3::Lerp(x.position, y.position, t),
					Math::Quaternion::Lerp(x.rotation, y.rotation, t),
					Math::Lerp(x.distance, y.distance, t)
				};
			}


			/* メンバ変数 */

			// 位置
			Math::Vector3    position;
			// 回転
			Math::Quaternion rotation;
			// ターゲットの距離
			float            distance;

		};
	}
}
