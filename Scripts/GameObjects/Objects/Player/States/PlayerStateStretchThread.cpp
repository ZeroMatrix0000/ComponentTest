/*
 * FileName:     PlayerStateStretchThread.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/15
 *
 * プレイヤーの地面にいる状態
 */

#include "Pch.h"
#include "PlayerStates.h"

#include "../PlayerThread.h"
#include "Scripts/Commons/Components/Transform.h"
#include "Scripts/Commons/Colliders/BoxCollider.h"
#include "Scripts/Commons/Renderings/Particle/ParticleManager.h"
#include "Scripts/GameObjects/Particles/JumpingParticle.h"

 // コンストラクタ
PlayerStateStretchThread::PlayerStateStretchThread(const Math::Vector3& targetPosition, PlayerInternals::TargetType targetType)
	: m_targetPosition{ targetPosition }
	, m_targetType{ targetType }
	, m_playerPosition{}
{
}

// 開始処理
void PlayerStateStretchThread::Enter(PlayerInternals* pInternals)
{
	m_playerPosition = pInternals->pBoxCollider->GetWorldBox().position;

	// 速度を 0 にする
	pInternals->moveVelocity = Math::Vector3::Zero;

	// 向き
	Math::Vector3 direction = m_targetPosition - m_playerPosition;
	direction.Normalize();
	// ターゲットの方向へ回転
	pInternals->rotation.SetTarget
	(
		Math::Quaternion::CreateFromRotationMatrix(Math::Matrix::CreateWorld(Math::Vector3::Zero, -direction, Math::Vector3::Up)),
		200.0f,
		25.0f
	);

	// ターゲットを設定
	pInternals->pThread->SetTarget(pInternals->pBoxCollider->GetWorldBox().position, m_targetPosition, pInternals->rotation.GetTarget());
}

// 更新処理
void PlayerStateStretchThread::Update(PlayerInternals* pInternals, float elapsedTime)
{
	// 回転の更新
	pInternals->rotation.Tick(elapsedTime);

	// 座標を調整
	pInternals->pTransform->SetPosition
	(
		m_playerPosition + Math::Vector3::Transform
		(
			Math::Vector3::Down * pInternals->pBoxCollider->GetWorldBox().size.y / 2.0f,
			pInternals->rotation.GetCurrent()
		)
	);

	// 回転させる
	pInternals->pTransform->SetRotation(pInternals->rotation.GetCurrent());

	// 当たり判定の更新
	pInternals->pBoxCollider->ApplyTransform();

	// 糸の更新
	pInternals->pThread->Update(elapsedTime);

	// パーティクルの更新
	pInternals->pParticleManager->Update(elapsedTime);
}

// 終了処理
void PlayerStateStretchThread::Exit(PlayerInternals* pInternals)
{
}

// 座標補正
void PlayerStateStretchThread::PositionCorrect(PlayerInternals* pInternals, float distance, const Math::Vector3& direction)
{
}
