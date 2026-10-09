/*
 * FileName:     PlayerModel.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/09/11
 *
 * プレイヤーのモデル
 */

#include "Pch.h"
#include "PlayerModel.h"

#include "Player.h"
#include "Scripts/Commons/Components/Transform.h"
#include "Scripts/Commons/GameObjects/GameObject.h"
#include "Scripts/Commons/Renderings/Particle/ParticleManager.h"
#include "Scripts/Commons/Systems/JsonSerializer.h"
#include "Scripts/GameObjects/Particles/JumpingParticle.h"

// コンストラクタ
PlayerModel::PlayerModel(const ComponentDesc& desc)
	: Component{ desc }
	, m_bounceHeight{}
	, m_fallSpeedCoefficient{}
	, m_scalingMax{}
	, m_dashAngleMax{}
	, m_bounceTime{}
	, m_fallingHeight{}
	, m_scaling{}
	, m_dashAngle{}
	, m_pTransform{ GetPOwner()->GetNullReferences<Transform>() }
	, m_pParticleManager{}
{
}

// 初期化処理
void PlayerModel::Initalize(const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder)
{
	float bounceTime = m_bounceTime.GetMax();

	Systems::JsonSerializer serializer{ pIGameObjectFinder };
	serializer.AddParameter(&bounceTime, "BounceTime");
	serializer.AddParameter(&m_bounceHeight, "BounceHeight");
	serializer.AddParameter(&m_fallSpeedCoefficient, "FallSpeedCoefficient");
	serializer.AddParameter(&m_scalingMax, "ScalingMax");
	serializer.AddParameter(&m_dashAngleMax, "DashAngleMax");
	serializer.Load(json);

	m_bounceTime.Initialize(0.0f, 0.0f, bounceTime);

	m_pTransform = GetPOwner()->GetComponent<Transform>();
}

// 更新処理
void PlayerModel::Update(float elapsedTime, const Player& player)
{
	switch (player.GetFallState())
	{
	case Player::FallState::Falling:
	{
		m_bounceTime = 0.0f;
		m_fallingHeight.SetTarget(0.0f);
		m_fallingHeight.Tick(elapsedTime);

		Math::Vector3 position = m_pTransform->GetPosition() + Math::Vector3::Up * m_fallingHeight.GetCurrent();
		m_pTransform->SetPosition(position);

		break;
	}
	default:
	{
		float bounceTime = m_bounceTime;
		m_bounceTime += elapsedTime;

		// 地面に着地しているタイミングでダッシュ中なら砂煙を再生
		if (bounceTime > m_bounceTime && player.GetMoveVelocityRatio() > 1.1f)
		{
			// 砂煙の再生
			m_pParticleManager->Play<JumpingParticle>();
		}
		
		// 跳ねる高さ
		float height = m_bounceHeight * Math::Sin(m_bounceTime.GetRatio() * Math::PI) * player.GetMoveVelocityRatio();
		m_fallingHeight.SetValue(height);
		// 座標
		Math::Vector3 position = m_pTransform->GetPosition() + Math::Vector3::Up * height;
		m_pTransform->SetPosition(position);

		break;
	}
	}

	// 落下速度でスケールを変更
	m_scaling.SetTarget(Math::Clamp(-player.GetFallSpeed() * m_fallSpeedCoefficient, -0.1f, 0.1f), 250.0f, 30.0f);
	m_scaling.Tick(elapsedTime);
	Math::Vector3 scale = m_pTransform->GetScale();
	scale.y += m_scaling.GetCurrent();
	scale.x -= m_scaling.GetCurrent() / 2.0f;
	scale.z -= m_scaling.GetCurrent() / 2.0f;
	m_pTransform->SetScale(scale);

	// 移動速度で回転
	m_dashAngle.SetTarget(Math::Clamp(player.GetMoveVelocityRatio() - 1.0f, 0.0f, 1.0f) * m_dashAngleMax);
	m_dashAngle.Tick(elapsedTime);
	m_pTransform->SetRotation(Math::Euler{ m_dashAngle.GetCurrent(), 0.0f, 0.0f }.CreateQuaternion() * m_pTransform->GetRotation());
}

// トランスフォームを設定
void PlayerModel::SetTransform(const Transform& transform)
{
	m_pTransform->SetPosition(transform.GetPosition());
	m_pTransform->SetRotation(transform.GetRotation());
	m_pTransform->SetScale(transform.GetScale());
}
