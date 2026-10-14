/*
 * FileName:     QuaternionCamera.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/14
 *
 * カメラ（四元数）
 */

#pragma once

namespace Libraries
{
	namespace Camera
	{
		// カメラ（四元数）
		struct QuaternionCamera
		{

			/* メンバ関数 */

			// コンストラクタ
			constexpr QuaternionCamera()
				: position{ Math::Vector3::Zero }
				, rotation{ Math::Quaternion::Identity }
			{
			}
			// コンストラクタ
			constexpr QuaternionCamera(const Math::Vector3& position, const Math::Quaternion& rotation)
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
			static QuaternionCamera Lerp(const QuaternionCamera& x, const QuaternionCamera& y, float t)
			{
				return QuaternionCamera{ Math::Vector3::Lerp(x.position, y.position, t), Math::Quaternion::Lerp(x.rotation, y.rotation, t) };
			}


			/* メンバ変数 */

			// 位置
			Math::Vector3    position;
			// 回転
			Math::Quaternion rotation;

		};
	}
}
