/*
 * FileName:     QuaternionCamera.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/14
 *
 * カメラ（四元数）
 */

#include "Pch.h"

// ビュー行列を作成
Math::Matrix Libraries::Camera::QuaternionCamera::CreateViewMatrix() const
{
	// カメラの上方向
	Math::Vector3 up        = Math::Vector3::Transform(Math::Vector3::Up, rotation);

	return Math::Matrix::CreateLookAt(position, position + GetEyeDirection(), up);
}

// 目線の方向を取得
Math::Vector3 Libraries::Camera::QuaternionCamera::GetEyeDirection() const
{
	return Math::Vector3::Transform(Math::Vector3::Forward, rotation);
}
