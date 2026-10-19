/*
 * FileName:     PlayerStateOnGround.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/16
 *
 * プレイヤーの地面にいる状態
 */

#include "Pch.h"
#include "PlayerStates.h"

#include "../PlayerInternals.h"
#include "../PlayerThread.h"
#include "Scripts/Commons/Components/Transform.h"
#include "Scripts/Commons/Colliders/BoxCollider.h"
#include "Scripts/Commons/Renderings/Particle/ParticleManager.h"
#include "Scripts/GameObjects/Particles/JumpingParticle.h"

// コンストラクタ
PlayerStateOnGround::PlayerStateOnGround()
	: m_isGround{}
{
}

// 開始処理
void PlayerStateOnGround::Enter(PlayerInternals* pInternals)
{
	pInternals->fallSpeed = 0.0f;
	pInternals->canExpandThread = true;
}

// 更新処理
void PlayerStateOnGround::Update(PlayerInternals* pInternals, float elapsedTime)
{
	m_isGround = false;

	// カメラのY軸回転
	Math::Quaternion cameraRotation = Math::Quaternion::CreateFromAxisAngle
	(
		Math::Vector3::UnitY,
		Math::Deg2Rad(pInternals->P_CAMERA_SCREEN->GetCamera().rotation.y)
	);

	// 最大の早さを調整
	pInternals->moveMaxSpeed += pInternals->MOVE_ACCELERATION * elapsedTime * pInternals->DASH_RATIO * (pInternals->IS_DASH ? 1.0f : -1.0f);

	// 加速度がない
	if (pInternals->MOVE == Math::Vector3::Zero)
	{
		// 速さ
		float speed = Math::Clamp
		(
			pInternals->moveVelocity.Length() - pInternals->MOVE_ACCELERATION * elapsedTime,
			0.0f,
			static_cast<float>(pInternals->moveMaxSpeed)
		);
		pInternals->moveVelocity.Normalize();
		pInternals->moveVelocity *= speed;
	}
	// 加速度がある
	else
	{
		// 速度に加速度を足す
		pInternals->moveVelocity += Math::Vector3::Transform(pInternals->MOVE, cameraRotation) *
			pInternals->MOVE_ACCELERATION * elapsedTime * (pInternals->IS_DASH ? pInternals->DASH_RATIO : 1.0f);
		// 速さを最大に収める
		if (pInternals->moveVelocity.Length() > pInternals->moveMaxSpeed)
		{
			pInternals->moveVelocity.Normalize();
			pInternals->moveVelocity *= pInternals->moveMaxSpeed;
		}

		pInternals->rotation.SetTarget
		(
			Math::Quaternion::CreateFromAxisAngle(Math::Vector3::UnitY, Math::Arctan2(pInternals->moveVelocity.x, pInternals->moveVelocity.z)),
			200.0f,
			25.0f
		);
	}

	// 落下速度を計算
	pInternals->fallSpeed += pInternals->FALL_ACCELERATION * elapsedTime;

	// ジャンプ入力
	if (pInternals->IS_JUMP)
	{
		pInternals->jumpBufferTime = pInternals->jumpBufferTime.GetMax();
	}
	pInternals->jumpBufferTime -= elapsedTime;

	// ジャンプ
	if (pInternals->jumpBufferTime != 0.0f)
	{
		SetNextState(std::make_unique<PlayerStateFalling>());
		pInternals->fallSpeed = -pInternals->JUMP_POWER;
		pInternals->jumpBufferTime = pInternals->jumpBufferTime.GetMin();
		// 砂煙の再生
		pInternals->pParticleManager->Play<JumpingParticle>();
	}

	// 糸の更新
	pInternals->pThread->Update(elapsedTime);

	// 回転の更新
	pInternals->rotation.Tick(elapsedTime);

	// 位置に速度を足す
	pInternals->pTransform->Translate((pInternals->moveVelocity - Math::Vector3::UnitY * pInternals->fallSpeed) * elapsedTime);
	// 回転させる
	pInternals->pTransform->SetRotation(pInternals->rotation.GetCurrent());

	// 当たり判定の更新
	pInternals->pBoxCollider->ApplyTransform();

	// パーティクルの更新
	pInternals->pParticleManager->Update(elapsedTime);
}

// 終了処理
void PlayerStateOnGround::Exit(PlayerInternals* pInternals)
{
}

// 座標補正
void PlayerStateOnGround::PositionCorrect(PlayerInternals* pInternals, float distance, const Math::Vector3& direction)
{
	float cos = direction.Dot(Math::Vector3::Up);

	// めり込んでいる、または、距離が近いかつ移動方向が下
	if (distance < 0.0f || (distance < pInternals->pBoxCollider->GetWorldBox().size.y / 8.0f && Math::Abs(cos) > 1.0f / 1.41421356f))
	{
		// 移動量
		Math::Vector3 displacement = direction * distance;

		// 移動方向の角度が 45° 以上なら真上に移動する
		if (Math::Abs(cos) > 1.0f / 1.41421356f)
		{
			displacement = Math::Vector3::Up / cos * distance;
			// 落下速度を 0 にする
			pInternals->fallSpeed = 0.0f;
		}

		// 移動
		pInternals->pTransform->Translate(displacement);
		// トランスフォームを適用
		pInternals->pBoxCollider->ApplyTransform();

		// 地面についていることを確定させる
		m_isGround = true;
		// 空中にいる状態になっているならリセット
		if (GetNextState())
		{
			ResetNextState();
		}
	}
	else
	{
		// 確定していないなら
		if (!m_isGround)
		{
			// 空中にいる状態にする
			SetNextState(std::make_unique<PlayerStateOnAir>());
		}
	}
}
