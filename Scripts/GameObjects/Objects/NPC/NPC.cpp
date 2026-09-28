/*
 * FileName:     NPC.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/09/28
 *
 * NPC
 */

#include "Pch.h"
#include "NPC.h"

#include "Scripts/Commons/GameObjects/GameObject.h"
#include "Scripts/Commons/GameObjects/IGameObjectFinder.h"
#include "Scripts/Commons/Components/Transform.h"
#include "Scripts/Commons/Colliders/BoxCollider.h"
#include "Scripts/Commons/Renderings/Text/Text.h"
#include "Scripts/Commons/Systems/JsonSerializer.h"

// コンストラクタ
NPC::NPC(const ComponentDesc& desc)
	: Component{ desc }
	, m_name{}
	, m_eventName{}
	, m_focusDistance{}
	, m_baseRotation{}
	, m_rotation{}
	, m_pTransform{ GetPOwner()->GetNullReferences<Transform>() }
	, m_pBoxCollider{ GetPOwner()->GetNullReferences<Colliders::BoxCollider>() }
{
}

// 初期化処理
void NPC::Initalize(const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder)
{
	m_pTransform = GetPOwner()->GetComponent<Transform>();
	m_pBoxCollider = GetPOwner()->GetComponent<Colliders::BoxCollider>();
	m_pBoxCollider->ApplyTransform();

	m_baseRotation = m_pTransform->GetRotation();
	m_rotation.SetValue(m_baseRotation);

	Systems::JsonSerializer serializer{ pIGameObjectFinder };
	serializer.AddParameter(&m_name, "Name");
	serializer.AddParameter(&m_eventName, "EventName");
	serializer.AddParameter(&m_focusDistance, "FocusDistance");
	serializer.Load(json);
}

// 更新処理
void NPC::Update(float elapsedTime)
{
	m_rotation.Tick(elapsedTime);

	m_pTransform->SetRotation(m_rotation.GetCurrent());
	m_pBoxCollider->ApplyTransform();
}

// 回転を設定
void NPC::SetRotation(const Math::Quaternion* rotation)
{
	// 回転を受け取らなかったら回転を基準に戻す
	if (!rotation)
	{
		m_rotation.SetTarget(m_baseRotation);
	}
	else
	{
		m_rotation.SetTarget(*rotation);
	}
}

// 中心座標を取得
const Math::Vector3& NPC::GetPosition() const
{
	return m_pBoxCollider->GetWorldBox().position;
}

// 当たり判定を取得
const Math::Box& NPC::GetBox() const
{
	return m_pBoxCollider->GetWorldBox();
}
