/*
 * FileName:     PlayerStateOnGround.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/12
 *
 * プレイヤーの地面にいる状態
 */

#include "Pch.h"
#include "PlayerStateOnGround.h"

#include "../PlayerInternals.h"
#include "Scripts/Commons/Components/Transform.h"
#include "Scripts/Commons/Colliders/BoxCollider.h"
#include "Scripts/Commons/Renderings/Particle/ParticleManager.h"
#include "Scripts/GameObjects/Particles/JumpingParticle.h"

// コンストラクタ
PlayerStateOnGround::PlayerStateOnGround()
{
}

// 開始処理
void PlayerStateOnGround::Enter(PlayerInternals* pInternals)
{
}

// 更新処理
void PlayerStateOnGround::Update(PlayerInternals* pInternals, float elapsedTime)
{
	// カメラのY軸回転
	Math::Quaternion cameraRotation = Math::Quaternion::CreateFromAxisAngle(Math::Vector3::UnitY, Math::Deg2Rad(pInternals->P_CAMERA_SCREEN->GetCamera().rotation.y));

	// 最大の早さを調整
	pInternals->moveMaxSpeed += pInternals->MOVE_ACCELERATION * elapsedTime * pInternals->DASH_RATIO * (pInternals->isDash ? 1.0f : -1.0f);

	// 加速度がない
	if (pInternals->move == Math::Vector3::Zero)
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
		pInternals->moveVelocity += Math::Vector3::Transform(pInternals->move, cameraRotation) * pInternals->MOVE_ACCELERATION * elapsedTime * (pInternals->isDash ? pInternals->DASH_RATIO : 1.0f);
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

	pInternals->fallSpeed += pInternals->FALL_ACCELERATION * elapsedTime;

	// ジャンプ入力
	if (pInternals->isJump)
	{
		pInternals->jumpBufferTime = pInternals->jumpBufferTime.GetMax();
	}

	pInternals->jumpBufferTime -= elapsedTime;

	// ジャンプ
	if (pInternals->jumpBufferTime != 0.0f)
	{
		//SetNextState<>();
		pInternals->fallState = PlayerInternals::FallState::Falling;
		pInternals->fallSpeed = pInternals->fallSpeed.GetMin();
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
}
