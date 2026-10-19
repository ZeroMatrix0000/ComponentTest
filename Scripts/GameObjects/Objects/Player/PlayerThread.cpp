/*
 * FileName:     PlayerThread.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/19
 *
 * プレイヤーの糸
 */

#include "Pch.h"
#include "PlayerThread.h"

#include "Scripts/GameObjects/Particles/PoppingParticle.h"
#include "Scripts/GameObjects/Particles/SparklingParticle.h"
#include "Scripts/GameObjects/Particles/WindCutParticle.h"
#include "Scripts/Commons/Components/Transform.h"
#include "Scripts/Commons/GameObjects/GameObject.h"
#include "Scripts/Commons/Renderings/Model3D/Model3D.h"
#include "Scripts/Commons/Renderings/Particle/ParticleManager.h"
#include "Scripts/Commons/Systems/JsonSerializer.h"

// コンストラクタ
PlayerThread::PlayerThread(const ComponentDesc& desc)
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
	, m_pTransform{ GetPOwner()->GetNullReferences<Transform>() }
	, m_pParticleManager{ GetPOwner()->GetNullReferences<Renderings::ParticleManager>() }
	, m_pParticleManagerTransform{ GetPOwner()->GetNullReferences<Transform>() }
{
}

// 初期化処理
void PlayerThread::Initalize(const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder)
{
	m_pTransform = GetPOwner()->GetComponent<Transform>();

	m_rotationAngle.Initialize(0.0f, -180.0f, 180.0f);

	m_currentState = State::None;

	Systems::JsonSerializer serializer{ pIGameObjectFinder };
	serializer.AddParameter(&THICKNESS, "Thickness");
	serializer.AddParameter(&EXPAND_SPEED, "ExpandSpeed");
	serializer.AddParameter(&ROTATION_SPEED, "RotationSpeed");
	serializer.AddParameter(&STAY_TIME, "StayTime");
	serializer.AddParameter(&CONTRACT_ACCELERATION, "ContractAcceleration");
	serializer.AddParameter(&MIN_LENGTH, "MinLength");
	serializer.Load(json);

	// パーティクル管理生成
	if (m_pParticleManager == GetPOwner()->GetNullReferences<Renderings::ParticleManager>())
	{
		GameObject* pObj = Instantiate("Prefab_PlayerThreadParticleManager");
		m_pParticleManager = pObj->GetComponent<Renderings::ParticleManager>();
		m_pParticleManagerTransform = pObj->GetComponent<Transform>();
	}
}

// 更新処理
void PlayerThread::Update(float elapsedTime)
{
	// 移動
	Translate(elapsedTime);
	// 回転
	Rotate(elapsedTime);
	// スケーリング
	Scaling(elapsedTime);

	m_pParticleManager->Update(elapsedTime);
}

// 移動
void PlayerThread::Translate(float elapsedTime)
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
				m_pParticleManagerTransform->SetPosition(m_currentPosition);
				m_pParticleManager->Play<PoppingParticle>();
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
			m_pParticleManager->Play<SparklingParticle>();
		}
		// パーティクルを出現
		if (m_contractSpeed > 32.0f)
		{
			m_particleTimer += elapsedTime;
			if (m_particleTimer >= WindCutParticle::INTERVAL)
			{
				m_particleTimer -= WindCutParticle::INTERVAL;
				m_pParticleManagerTransform->SetPosition(m_playerPosition);
				m_pParticleManager->Play<WindCutParticle>();
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
	m_pTransform->SetPosition((m_playerPosition + m_currentPosition) / 2.0f);
}

// 回転
void PlayerThread::Rotate(float elapsedTime)
{
	// 伸ばし状態なら回転させる
	if (m_currentState == State::Expand)
	{
		m_rotationAngle += ROTATION_SPEED * elapsedTime;
	}

	// 回転
	m_pTransform->SetRotation(Math::Euler{ 0.0f, 0.0f, m_rotationAngle }.CreateQuaternion() * m_baseRotation);
	m_pParticleManagerTransform->SetRotation(Math::Euler{ 0.0f, 0.0f, m_rotationAngle }.CreateQuaternion() * m_baseRotation);
}

// スケーリング
void PlayerThread::Scaling(float elapsedTime)
{
	if (m_currentState == State::None && !m_isDisplay)
	{
		m_pTransform->SetScale(Math::Vector3::Zero);
	}
	else
	{
		m_pTransform->SetScale(Math::Vector3{ THICKNESS, THICKNESS, m_length });
	}
}

// 映るカメラ画面を追加
void PlayerThread::AddICameraScreen(const Renderings::ICameraScreen& iCameraScreen)
{
	GetPOwner()->GetComponent<Renderings::Model3D>()->AddICameraScreen(iCameraScreen);
	m_pParticleManager->AddICameraScreen(iCameraScreen);
}

// ターゲットを設定
void PlayerThread::SetTarget(const Math::Vector3& playerPosition, const Math::Vector3& targetPosition, const Math::Quaternion& rotation, bool toAir)
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
