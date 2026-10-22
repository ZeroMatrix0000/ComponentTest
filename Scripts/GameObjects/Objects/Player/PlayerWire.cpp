/*
 * FileName:     PlayerWire.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/22
 *
 * プレイヤーの糸
 */

#include "Pch.h"
#include "PlayerWire.h"

#include "Scripts/GameObjects/Particles/PoppingParticle.h"
#include "Scripts/GameObjects/Particles/SparklingParticle.h"
#include "Scripts/GameObjects/Particles/WindCutParticle.h"
#include "Scripts/Commons/Components/Transform.h"
#include "Scripts/Commons/GameObjects/GameObject.h"
#include "Scripts/Commons/Renderings/Model3D/Model3D.h"
#include "Scripts/Commons/Renderings/Particle/ParticleManager.h"
#include "Scripts/Commons/Systems/JsonSerializer.h"

// コンストラクタ
PlayerWire::PlayerWire(const ComponentDesc& desc)
	: Component{ desc }
	, THICKNESS{}
	, EXPAND_SPEED{}
	, ROTATION_SPEED{}
	, STAY_TIME{}
	, CONTRACT_ACCELERATION{}
	, MIN_LENGTH{}
	, m_distance{}
	, m_length{}
	, m_playerPosition{}
	, m_targetPosition{}
	, m_currentPosition{}
	, m_direction{}
	, m_baseRotation{}
	, m_rotationAngle{}
	, m_stayTimer{}
	, m_contractSpeed{}
	, m_currentState{}
	, m_toAir{}
	, m_particleTimer{}
	, m_isDisplay{}
	, m_refTransform{ GetOwner()->GetNullReference<Transform>() }
	, m_refParticleManager{ GetOwner()->GetNullReference<Renderings::ParticleManager>() }
	, m_refParticleManagerTransform{ GetOwner()->GetNullReference<Transform>() }
{
}

// 初期化処理
void PlayerWire::Initalize(const nlohmann::ordered_json& json, Ref<IGameObjectFinder> iGameObjectFinder)
{
	m_refTransform = GetOwner()->GetComponent<Transform>();

	m_rotationAngle.Initialize(0.0f, -180.0f, 180.0f);

	m_currentState = State::None;

	Systems::JsonSerializer serializer{ iGameObjectFinder };
	serializer.AddParameter(&THICKNESS, "Thickness");
	serializer.AddParameter(&EXPAND_SPEED, "ExpandSpeed");
	serializer.AddParameter(&ROTATION_SPEED, "RotationSpeed");
	serializer.AddParameter(&STAY_TIME, "StayTime");
	serializer.AddParameter(&CONTRACT_ACCELERATION, "ContractAcceleration");
	serializer.AddParameter(&MIN_LENGTH, "MinLength");
	serializer.Load(json);

	// パーティクル管理生成
	if (m_refParticleManager == GetOwner()->GetNullReference<Renderings::ParticleManager>())
	{
		GameObject& obj = Instantiate("Prefab_PlayerWireParticleManager");
		m_refParticleManager = obj.GetComponent<Renderings::ParticleManager>();
		m_refParticleManagerTransform = obj.GetComponent<Transform>();
	}
}

// 更新処理
void PlayerWire::Update(float elapsedTime)
{
	// 移動
	Translate(elapsedTime);
	// 回転
	Rotate(elapsedTime);
	// スケーリング
	Scaling(elapsedTime);

	m_refParticleManager->Update(elapsedTime);
}

// 移動
void PlayerWire::Translate(float elapsedTime)
{
	switch (m_currentState)
	{
	case State::Expand:
		// 現在位置を計算
		m_currentPosition += m_direction * EXPAND_SPEED * elapsedTime;
		// 糸の長さ
		m_length += EXPAND_SPEED * elapsedTime;
		// 糸の長さが距離以上になったら
		if (m_length >= m_distance)
		{
			m_length = m_distance;
			m_currentPosition = m_targetPosition;
			m_currentState = m_toAir ? State::AirContract : State::Stay;
			// パーティクルの再生
			if (!m_toAir)
			{
				m_refParticleManagerTransform->SetPosition(m_currentPosition);
				m_refParticleManager->Play<PoppingParticle>();
			}
		}
		break;
	case State::Stay:
		m_stayTimer += elapsedTime;
		if (m_stayTimer >= STAY_TIME)
		{
			m_currentState = State::Contract;
		}
		break;
	case State::Contract:
		// 速度を計算
		m_contractSpeed += CONTRACT_ACCELERATION * elapsedTime;
		// プレイヤー座標を計算
		m_playerPosition += m_direction * m_contractSpeed * elapsedTime;
		// 糸の長さ
		m_length -= m_contractSpeed * elapsedTime;
		// 糸が短くなったら
		if (m_length <= MIN_LENGTH)
		{
			m_playerPosition = m_currentPosition - m_direction * MIN_LENGTH;
			m_currentState = State::None;
			// パーティクルの再生
			m_refParticleManager->Play<SparklingParticle>();
		}
		// パーティクルを出現
		if (m_contractSpeed > 32.0f)
		{
			m_particleTimer += elapsedTime;
			if (m_particleTimer >= WindCutParticle::INTERVAL)
			{
				m_particleTimer -= WindCutParticle::INTERVAL;
				m_refParticleManagerTransform->SetPosition(m_playerPosition);
				m_refParticleManager->Play<WindCutParticle>();
			}
		}
		break;
	case State::AirContract:
		// 現在位置を計算
		m_currentPosition -= m_direction * EXPAND_SPEED * elapsedTime;
		// 糸の長さ
		m_length -= EXPAND_SPEED * elapsedTime;
		// 糸が短くなったら
		if (m_length <= MIN_LENGTH)
		{
			m_playerPosition = m_currentPosition - m_direction * MIN_LENGTH;
			m_currentState = State::None;
		}
		break;
	default:
		break;
	}

	// 移動
	m_refTransform->SetPosition((m_playerPosition + m_currentPosition) / 2.0f);
}

// 回転
void PlayerWire::Rotate(float elapsedTime)
{
	// 伸ばし状態なら回転させる
	if (m_currentState == State::Expand)
	{
		m_rotationAngle += ROTATION_SPEED * elapsedTime;
	}

	// 回転
	m_refTransform->SetRotation(Math::Euler{ 0.0f, 0.0f, m_rotationAngle }.CreateQuaternion() * m_baseRotation);
	m_refParticleManagerTransform->SetRotation(Math::Euler{ 0.0f, 0.0f, m_rotationAngle }.CreateQuaternion() * m_baseRotation);
}

// スケーリング
void PlayerWire::Scaling(float elapsedTime)
{
	if (m_currentState == State::None && !m_isDisplay)
	{
		m_refTransform->SetScale(Math::Vector3::Zero);
	}
	else
	{
		m_refTransform->SetScale(Math::Vector3{ THICKNESS, THICKNESS, m_length });
	}
}

// 映るカメラ画面を追加
void PlayerWire::AddICameraScreen(Ref<const Renderings::ICameraScreen> iCameraScreen)
{
	GetOwner()->GetComponent<Renderings::Model3D>()->AddICameraScreen(iCameraScreen);
	m_refParticleManager->AddICameraScreen(iCameraScreen);
}

// ターゲットを設定
void PlayerWire::SetTarget(const Math::Vector3& playerPosition, const Math::Vector3& targetPosition, const Math::Quaternion& rotation, bool toAir)
{
	m_distance = (targetPosition - playerPosition).Length();
	m_length = 0.0f;
	m_playerPosition = playerPosition;
	m_targetPosition = targetPosition;
	m_currentPosition = m_playerPosition;
	m_direction = targetPosition - playerPosition;
	m_direction.Normalize();
	m_baseRotation = rotation;
	m_stayTimer = 0.0f;
	m_contractSpeed = 0.0f;
	m_currentState = State::Expand;
	m_toAir = toAir;
	m_isDisplay = true;
}

// プレイヤーの座標を設定
void PlayerWire::SetPlayerPosition(const Math::Vector3& playerPosition)
{
	m_playerPosition = playerPosition;

	m_direction = m_targetPosition - m_playerPosition;
	m_length = m_direction.Length();
	m_direction /= m_length;

	m_baseRotation = Math::Quaternion::CreateFromRotationMatrix
	(
		Math::Matrix::CreateWorld(Math::Vector3::Zero, -m_direction, Math::Vector3::Up)
	);
}
