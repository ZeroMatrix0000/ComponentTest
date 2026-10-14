/*
 * FileName:     QuaternionTargetCamera.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/14
 *
 * ターゲットカメラ（四元数）
 */

#include "Pch.h"

Math::Matrix Libraries::Camera::QuaternionTargetCamera::CreateViewMatrix() const
{
	// カメラの上方向
	Math::Vector3 up        = Math::Vector3::Transform(Math::Vector3::Up, rotation);

	return Math::Matrix::CreateLookAt(position - GetEyeDirection() * distance, position, up);
}

// 目線の位置を取得
Math::Vector3 Libraries::Camera::QuaternionTargetCamera::GetEyePosition() const
{
	// カメラの向き
	Math::Vector3    direction = Math::Vector3::Transform(Math::Vector3::Forward, rotation);

	return position - direction * distance;
}

// 目線の方向を取得
Math::Vector3 Libraries::Camera::QuaternionTargetCamera::GetEyeDirection() const
{
	return Math::Vector3::Transform(Math::Vector3::Forward, rotation);
}
