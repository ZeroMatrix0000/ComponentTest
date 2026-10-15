/*
 * FileName:     PlayerThread.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/15
 *
 * プレイヤーの糸
 */

#include "Pch.h"
#include "PlayerThread.h"

#include "Scripts/Commons/Components/Transform.h"
#include "Scripts/Commons/GameObjects/GameObject.h"
#include "Scripts/Commons/Systems/JsonSerializer.h"

// コンストラクタ
PlayerThread::PlayerThread(const ComponentDesc& desc)
	: Component{ desc }
	, m_thickness{}
	, m_stretchSpeed{}
	, m_rotationSpeed{}
	, m_distance{}
	, m_playerPosition{}
	, m_targetPosition{}
	, m_currentPosition{}
	, m_direction{}
	, m_baseRotation{}
	, m_rotationAngle{}
	, m_pTransform{ GetPOwner()->GetNullReferences<Transform>() }
{
}

// 初期化処理
void PlayerThread::Initalize(const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder)
{
	m_pTransform = GetPOwner()->GetComponent<Transform>();

	m_rotationAngle.Initialize(0.0f, -180.0f, 180.0f);

	Systems::JsonSerializer serializer{ pIGameObjectFinder };
	serializer.AddParameter(&m_thickness, "Thickness");
	serializer.AddParameter(&m_stretchSpeed, "StretchSpeed");
	serializer.AddParameter(&m_rotationSpeed, "RotationSpeed");
	serializer.Load(json);
}

// 更新処理
void PlayerThread::Update(float elapsedTime)
{
	// 現在位置を計算
	m_currentPosition += m_direction * m_stretchSpeed * elapsedTime;
	// 糸の長さ
	float length = (m_currentPosition - m_playerPosition).Length();
	// 糸の長さが距離を超えたら
	if (length > m_distance)
	{
		m_currentPosition = m_targetPosition;
	}

	// 移動
	m_pTransform->SetPosition((m_playerPosition + m_currentPosition) / 2.0f);

	// 回転
	m_rotationAngle += m_rotationSpeed * elapsedTime;
	m_pTransform->SetRotation(Math::Euler{ 0.0f, 0.0f, m_rotationAngle }.CreateQuaternion() * m_baseRotation);

	// スケーリング
	m_pTransform->SetScale(Math::Vector3{ m_thickness, m_thickness, length });
}

// ターゲットを設定
void PlayerThread::SetTarget(const Math::Vector3& playerPosition, const Math::Vector3& targetPosition, const Math::Quaternion& rotation)
{
	m_playerPosition = playerPosition;
	m_targetPosition = targetPosition;
	m_distance = (targetPosition - playerPosition).Length();
	m_currentPosition = m_playerPosition;
	m_direction = targetPosition - playerPosition;
	m_direction.Normalize();
	m_baseRotation = rotation;
}
