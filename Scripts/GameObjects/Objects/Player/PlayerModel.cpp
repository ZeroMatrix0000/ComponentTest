/*
 * FileName:     PlayerModel.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/21
 *
 * プレイヤーのモデル
 */

#include "Pch.h"
#include "PlayerModel.h"

#include "Player.h"
#include "States/PlayerStates.h"
#include "Scripts/Commons/Components/Transform.h"
#include "Scripts/Commons/GameObjects/GameObject.h"
#include "Scripts/Commons/Renderings/Model3D/Model3D.h"
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
	, m_wireAngle{}
	, m_refTransform{ GetOwner().GetNullReferences<Transform>() }
	, m_refSilhouette{ GetOwner().GetNullReferences<Transform>() }
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

	m_refTransform = GetOwner().GetComponent<Transform>();

	// シルエットを生成
	if (m_refSilhouette == GetOwner().GetNullReferences<Transform>())
	{
		GameObject& obj = Instantiate("Prefab_PlayerSilhouette");
		m_refSilhouette = obj.GetComponent<Transform>();
	}
}

// 更新処理
void PlayerModel::Update(float elapsedTime, const Player& player)
{
	// 移動
	Translate(elapsedTime, player);
	// 回転
	Rotate(elapsedTime, player);
	// スケーリング
	Scaling(elapsedTime, player);
}

// 移動
void PlayerModel::Translate(float elapsedTime, const Player& player)
{
	// 落下状態 or 糸伸ばし状態
	if (player.IsState<PlayerStateFalling>())
	{
		m_bounceTime = 0.0f;
		m_fallingHeight.SetTarget(0.0f);
		m_fallingHeight.Tick(elapsedTime);

		Math::Vector3 position = player.GetBox().position + Math::Vector3::Up * m_fallingHeight.GetCurrent() + Math::Vector3::Transform
		(
			Math::Vector3::Down * player.GetBox().size.y / 2.0f,
			Math::Euler{ m_wireAngle.GetCurrent(), 0.0f, 0.0f }.CreateQuaternion() * m_refTransform->GetRotation()
		);
		m_refTransform->SetPosition(position);
		m_refSilhouette->SetPosition(position);
	}
	// 糸伸ばし状態
	else if (player.IsState<PlayerStateExpandWire>())
	{
		m_bounceTime = 0.0f;
		m_fallingHeight.SetTarget(0.0f);
		m_fallingHeight.Tick(elapsedTime);

		Math::Vector3 position = player.GetBox().position + Math::Vector3::Up * m_fallingHeight.GetCurrent() + Math::Vector3::Transform
		(
			Math::Vector3::Down * player.GetBox().size.y / 2.0f,
			Math::Euler{m_wireAngle.GetCurrent(), 0.0f, 0.0f }.CreateQuaternion() * m_refTransform->GetRotation()
		);
		m_refTransform->SetPosition(position);
		m_refSilhouette->SetPosition(position);
	}
	else
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
		Math::Vector3 position = player.GetBox().position + Math::Vector3::Up * height + Math::Vector3::Transform
		(
			Math::Vector3::Down * player.GetBox().size.y / 2.0f,
			Math::Euler{ m_wireAngle.GetCurrent(), 0.0f, 0.0f }.CreateQuaternion() * m_refTransform->GetRotation()
		);
		m_refTransform->SetPosition(position);
		m_refSilhouette->SetPosition(position);
	}
}

// 回転
void PlayerModel::Rotate(float elapsedTime, const Player& player)
{
	// 糸伸ばし状態
	if (player.IsState<PlayerStateExpandWire>())
	{
		m_dashAngle.SetTarget(0.0f);
	}
	else
	{
		// 移動速度で回転
		m_dashAngle.SetTarget(Math::Clamp(player.GetMoveVelocityRatio() - 1.0f, 0.0f, 1.0f) * m_dashAngleMax, 200.0f, 25.0f);
		m_wireAngle.SetTarget(0.0f);
	}

	m_dashAngle.Tick(elapsedTime);
	m_wireAngle.Tick(elapsedTime);

	float rotationX = m_dashAngle.GetCurrent() + m_wireAngle.GetCurrent();
	m_refTransform->SetRotation(Math::Euler{ rotationX, 0.0f, 0.0f }.CreateQuaternion() * m_refTransform->GetRotation());
	m_refSilhouette->SetRotation(Math::Euler{ rotationX, 0.0f, 0.0f }.CreateQuaternion() * m_refSilhouette->GetRotation());
}

// スケーリング
void PlayerModel::Scaling(float elapsedTime, const Player& player)
{
	// 糸伸ばし状態
	if (player.IsState<PlayerStateExpandWire>())
	{
		m_scaling.SetTarget(Math::Vector3::Zero, 250.0f, 30.0f);
	}
	else
	{
		// 落下によるスケーリングの大きさ
		float fallScaling = Math::Clamp(-player.GetFallSpeed() * m_fallSpeedCoefficient, -0.1f, 0.1f);
		// 落下速度でスケールを変更
		m_scaling.SetTarget(Math::Vector3{ -fallScaling / 2.0f, fallScaling, -fallScaling / 2.0f }, 250.0f, 30.0f);
	}

	m_scaling.Tick(elapsedTime);
	Math::Vector3 scale = m_refTransform->GetScale() + m_scaling.GetCurrent();
	m_refTransform->SetScale(scale);
	m_refSilhouette->SetScale(scale);
}

// 映るカメラ画面を追加
void PlayerModel::AddICameraScreen(const Renderings::ICameraScreen& iCameraScreen)
{
	GetOwner().GetComponent<Renderings::Model3D>().AddICameraScreen(iCameraScreen);
	m_refSilhouette->GetOwner().GetComponent<Renderings::Model3D>().AddICameraScreen(iCameraScreen);
}

// トランスフォームを設定
void PlayerModel::SetTransform(const Transform& transform)
{
	m_refTransform->SetPosition(transform.GetPosition());
	m_refTransform->SetRotation(transform.GetRotation());
	m_refTransform->SetScale(transform.GetScale());
	m_refSilhouette->SetPosition(transform.GetPosition());
	m_refSilhouette->SetRotation(transform.GetRotation());
	m_refSilhouette->SetScale(transform.GetScale());
}
