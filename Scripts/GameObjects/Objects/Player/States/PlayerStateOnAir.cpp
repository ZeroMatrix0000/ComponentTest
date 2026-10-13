/*
 * FileName:     PlayerStateOnAir.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/13
 *
 * プレイヤーの空中にいる状態
 */

#include "Pch.h"
#include "PlayerStates.h"

#include "../PlayerInternals.h"
#include "Scripts/Commons/Components/Transform.h"
#include "Scripts/Commons/Colliders/BoxCollider.h"
#include "Scripts/Commons/Renderings/Particle/ParticleManager.h"
#include "Scripts/GameObjects/Particles/JumpingParticle.h"

 // コンストラクタ
PlayerStateOnAir::PlayerStateOnAir()
{
}

// 開始処理
void PlayerStateOnAir::Enter(PlayerInternals* pInternals)
{
	pInternals->fallCoyoteTime = pInternals->fallCoyoteTime.GetMax();
}

// 更新処理
void PlayerStateOnAir::Update(PlayerInternals* pInternals, float elapsedTime)
{
	// カメラのY軸回転
	Math::Quaternion cameraRotation = Math::Quaternion::CreateFromAxisAngle(Math::Vector3::UnitY, Math::Deg2Rad(pInternals->P_CAMERA_SCREEN->GetCamera().rotation.y));

	// 最大の早さを調整
	pInternals->moveMaxSpeed += pInternals->MOVE_ACCELERATION * elapsedTime * pInternals->DASH_RATIO * (pInternals->IS_DASH ? 1.0f : -1.0f);

	// 加速度がない
	if (pInternals->MOVE == Math::Vector3::Zero)
	{
		// 速さ
		Limited speed = Limited::Create(pInternals->moveVelocity.Length() - pInternals->MOVE_ACCELERATION * elapsedTime, 0.0f, pInternals->moveMaxSpeed);
		pInternals->moveVelocity.Normalize();
		pInternals->moveVelocity *= speed;
	}
	// 加速度がある
	else
	{
		// 速度に加速度を足す
		pInternals->moveVelocity += Math::Vector3::Transform(pInternals->MOVE, cameraRotation) * pInternals->MOVE_ACCELERATION * elapsedTime * (pInternals->IS_DASH ? pInternals->DASH_RATIO : 1.0f);
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
		pInternals->fallSpeed = pInternals->fallSpeed.GetMin();
		pInternals->jumpBufferTime = pInternals->jumpBufferTime.GetMin();
		// 砂煙の再生
		pInternals->pParticleManager->Play<JumpingParticle>();
	}

	// 回転の更新
	pInternals->rotation.Tick(elapsedTime);

	// 位置に速度を足す
	pInternals->pTransform->Translate((pInternals->moveVelocity - Math::Vector3::UnitY * pInternals->fallSpeed) * elapsedTime);
	// 回転させる
	pInternals->pTransform->SetRotation(pInternals->rotation.GetCurrent());

	// 落下猶予
	pInternals->fallCoyoteTime -= elapsedTime;
	if (pInternals->fallCoyoteTime.IsMin())
	{
		 SetNextState(std::make_unique<PlayerStateFalling>());
	}

	// 当たり判定の更新
	pInternals->pBoxCollider->ApplyTransform();

	// パーティクルの更新
	pInternals->pParticleManager->Update(elapsedTime);
}

// 終了処理
void PlayerStateOnAir::Exit(PlayerInternals* pInternals)
{
}

// 座標補正
void PlayerStateOnAir::PositionCorrect(PlayerInternals* pInternals, float distance, const Math::Vector3& direction)
{
	if (distance < 0.0f)
	{
		float arccos = direction.Dot(Math::Vector3::Up);

		// 移動量
		Math::Vector3 displacement = direction * distance;
		// 移動方向の角度が 45° 以上なら真上に移動する
		if (Math::Abs(arccos) > 1.0f / 1.41421356f)
		{
			displacement = Math::Vector3::Up / arccos * distance;
			// 地面にいる状態にする
			SetNextState(std::make_unique<PlayerStateOnGround>());
		}
		// 移動
		pInternals->pTransform->Translate(displacement);
		// トランスフォームを適用
		pInternals->pBoxCollider->ApplyTransform();
	}
}
