/*
 * FileName:     PlayerStateExpandWire.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/19
 *
 * プレイヤーの地面にいる状態
 */

#include "Pch.h"
#include "PlayerStates.h"

#include "../PlayerWire.h"
#include "../PlayerModel.h"
#include "Scripts/Commons/Components/Transform.h"
#include "Scripts/Commons/Colliders/BoxCollider.h"
#include "Scripts/Commons/Renderings/Particle/ParticleManager.h"

 // コンストラクタ
PlayerStateExpandWire::PlayerStateExpandWire(const Math::Vector3& targetPosition, PlayerInternals::TargetType targetType)
	: m_targetPosition{ targetPosition }
	, m_targetType{ targetType }
	, m_playerPosition{}
{
}

// 開始処理
void PlayerStateExpandWire::Enter(PlayerInternals* pInternals)
{
	m_playerPosition = pInternals->pBoxCollider->GetWorldBox().position;

	// 速度を 0 にする
	pInternals->moveVelocity = Math::Vector3::Zero;
	pInternals->fallSpeed = 0.0f;

	// 向き
	Math::Vector3 direction = m_targetPosition - m_playerPosition;
	direction.Normalize();

	// モデルを回転
	pInternals->pModel->SetWireAngle(Math::Rad2Deg(Math::Arcsin(-direction.y / direction.Length())));

	Math::Quaternion wireRotation = Math::Quaternion::CreateFromRotationMatrix
	(
		Math::Matrix::CreateWorld(Math::Vector3::Zero, -direction, Math::Vector3::Up)
	);
	direction.y = 0;

	// ターゲットの方向へ回転
	pInternals->rotation.SetTarget
	(
		Math::Quaternion::CreateFromRotationMatrix(Math::Matrix::CreateWorld(Math::Vector3::Zero, -direction, Math::Vector3::Up)),
		200.0f,
		25.0f
	);

	// ターゲットを設定
	pInternals->pWire->SetTarget
	(
		pInternals->pBoxCollider->GetWorldBox().position,
		m_targetPosition,
		wireRotation,
		m_targetType == PlayerInternals::TargetType::Air
	);
}

// 更新処理
void PlayerStateExpandWire::Update(PlayerInternals* pInternals, float elapsedTime)
{
	// 糸の更新
	pInternals->pWire->Update(elapsedTime);

	// 回転の更新
	pInternals->rotation.Tick(elapsedTime);

	// 座標を調整
	m_playerPosition = pInternals->pWire->GetPlayerPosition();
	pInternals->pTransform->SetPosition(m_playerPosition + Math::Vector3::Down * pInternals->pBoxCollider->GetWorldBox().size.y / 2.0f);

	// 回転させる
	pInternals->pTransform->SetRotation(pInternals->rotation.GetCurrent());

	// 当たり判定の更新
	pInternals->pBoxCollider->ApplyTransform();

	// パーティクルの更新
	pInternals->pParticleManager->Update(elapsedTime);

	// 落下状態に戻す
	if (pInternals->pWire->GetCurrentState() == PlayerWire::State::None)
	{
		switch (m_targetType)
		{
		case PlayerInternals::TargetType::Object:
			SetNextState(std::make_unique<PlayerStateFalling>());
			pInternals->fallSpeed = pInternals->fallSpeed.GetMin();
			pInternals->canExpandWire = true;
			pInternals->pWire->UnDisplay();
			break;
		case PlayerInternals::TargetType::Ground:
			SetNextState(std::make_unique<PlayerStateStick>());
			break;
		case PlayerInternals::TargetType::Air:
			SetNextState(std::make_unique<PlayerStateFalling>());
			pInternals->pWire->UnDisplay();
			break;
		default:
			break;
		}
	}
}

// 終了処理
void PlayerStateExpandWire::Exit(PlayerInternals* pInternals)
{
}

// 座標補正
void PlayerStateExpandWire::PositionCorrect(PlayerInternals* pInternals, float distance, const Math::Vector3& direction)
{
}
