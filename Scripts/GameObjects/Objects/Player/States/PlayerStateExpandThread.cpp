/*
 * FileName:     PlayerStateExpandThread.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/16
 *
 * プレイヤーの地面にいる状態
 */

#include "Pch.h"
#include "PlayerStates.h"

#include "../PlayerThread.h"
#include "Scripts/GameObjects/Particles/JumpingParticle.h"
#include "Scripts/Commons/Components/Transform.h"
#include "Scripts/Commons/Colliders/BoxCollider.h"
#include "Scripts/Commons/Renderings/Particle/ParticleManager.h"

 // コンストラクタ
PlayerStateExpandThread::PlayerStateExpandThread(const Math::Vector3& targetPosition, PlayerInternals::TargetType targetType)
	: m_targetPosition{ targetPosition }
	, m_targetType{ targetType }
	, m_playerPosition{}
{
}

// 開始処理
void PlayerStateExpandThread::Enter(PlayerInternals* pInternals)
{
	m_playerPosition = pInternals->pBoxCollider->GetWorldBox().position;

	// 速度を 0 にする
	pInternals->moveVelocity = Math::Vector3::Zero;
	pInternals->fallSpeed = 0.0f;

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
	pInternals->pThread->SetTarget
	(
		pInternals->pBoxCollider->GetWorldBox().position,
		m_targetPosition,
		pInternals->rotation.GetTarget(),
		m_targetType == PlayerInternals::TargetType::Air
	);
}

// 更新処理
void PlayerStateExpandThread::Update(PlayerInternals* pInternals, float elapsedTime)
{
	// 糸伸ばしタイマーの更新
	pInternals->threadTimer -= elapsedTime;

	// 糸の更新
	pInternals->pThread->Update(elapsedTime);

	// 回転の更新
	pInternals->rotation.Tick(elapsedTime);

	// 座標を調整
	m_playerPosition = pInternals->pThread->GetPlayerPosition();
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

	// パーティクルの更新
	pInternals->pParticleManager->Update(elapsedTime);

	// 落下状態に戻す
	if (pInternals->pThread->GetCurrentState() == PlayerThread::State::None)
	{
		switch (m_targetType)
		{
		case PlayerInternals::TargetType::Object:
			SetNextState(std::make_unique<PlayerStateFalling>());
			pInternals->fallSpeed = pInternals->fallSpeed.GetMin();
			break;
		case PlayerInternals::TargetType::Ground:
			SetNextState(std::make_unique<PlayerStateFalling>());
			pInternals->fallSpeed = pInternals->fallSpeed.GetMin();
			break;
		case PlayerInternals::TargetType::Air:
			SetNextState(std::make_unique<PlayerStateFalling>());
			break;
		default:
			break;
		}
	}
}

// 終了処理
void PlayerStateExpandThread::Exit(PlayerInternals* pInternals)
{
	// 向き
	Math::Vector3 direction = m_targetPosition - m_playerPosition;
	direction.y = 0;
	// 回転
	pInternals->rotation.SetTarget
	(
		Math::Quaternion::CreateFromRotationMatrix(Math::Matrix::CreateWorld(Math::Vector3::Zero, -direction, Math::Vector3::Up)),
		200.0f,
		25.0f
	);
}

// 座標補正
void PlayerStateExpandThread::PositionCorrect(PlayerInternals* pInternals, float distance, const Math::Vector3& direction)
{
}
