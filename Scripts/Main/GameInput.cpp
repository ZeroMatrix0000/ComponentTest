/*
 * FileName:     GameInput.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/14
 *
 * ゲームの入力
 */

#include "Pch.h"
#include "GameInput.h"

 // コンストラクタ
GameInput::GameInput(const Systems::IInput& input)
	: IGameInput{}
	, m_input{ input }
{
}

// プレイヤーの移動入力を取得
Math::Vector3 GameInput::GetPlayerMove() const
{
	// 方向
	Math::Vector3 direction{};

	// 上
	if (m_input.GetKey(KeyName::W) || m_input.GetKey(KeyName::Up))
	{
		direction += Math::Vector3::Forward;
	}
	// 左
	if (m_input.GetKey(KeyName::A) || m_input.GetKey(KeyName::Left))
	{
		direction += Math::Vector3::Left;
	}
	// 下
	if (m_input.GetKey(KeyName::S) || m_input.GetKey(KeyName::Down))
	{
		direction += Math::Vector3::Backward;
	}
	// 右
	if (m_input.GetKey(KeyName::D) || m_input.GetKey(KeyName::Right))
	{
		direction += Math::Vector3::Right;
	}

	// 極小値ならゼロに
	if (direction.Length() < Math::EPSILON)
	{
		return Math::Vector3::Zero;
	}
	// でなければ正規化
	else
	{
		direction.Normalize();
		return direction;
	}
}

// カメラの回転を取得
Math::Vector2 GameInput::GetPlayerCameraRotate() const
{
	return m_input.GetMouseMovement() * MOUSE_COEFFICIENT;
}

// プレイヤーのダッシュ入力を取得
bool GameInput::GetPlayerDash() const
{
	return m_input.GetKey(KeyName::LeftShift);
}

// プレイヤーのジャンプ入力を取得
bool GameInput::GetPlayerJump() const
{
	return m_input.GetKeyDown(KeyName::Space);
}

// プレイヤーの糸伸ばし入力を取得
bool GameInput::GetPlayerStretchThread() const
{
	return m_input.GetMouseButtonDown(MouseButtonName::Right);
}

// フィールド上のオブジェクトのインタラクト入力を取得
bool GameInput::GetFieldInteract() const
{
	return m_input.GetKeyDown(KeyName::Z);
}

// テキスト送り入力
bool GameInput::GetTextScroll() const
{
	return m_input.GetKeyDown(KeyName::Z) || m_input.GetKeyDown(KeyName::X);
}
