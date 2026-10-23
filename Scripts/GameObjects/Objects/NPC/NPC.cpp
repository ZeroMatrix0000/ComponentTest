/*
 * FileName:     NPC.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/23
 *
 * NPC
 */

#include "Pch.h"
#include "NPC.h"

#include "Scripts/Commons/GameObjects/GameObject.h"
#include "Scripts/Commons/GameObjects/IGameObjectFinder.h"
#include "Scripts/Commons/Components/Transform.h"
#include "Scripts/Commons/Colliders/BoxCollider.h"
#include "Scripts/Commons/Colliders/SphereCollider.h"
#include "Scripts/Commons/Renderings/Text/Text.h"
#include "Scripts/Commons/Systems/JsonSerializer.h"
#include "Scripts/Commons/Systems/IsDerivedRegister.h"

// コンストラクタ
NPC::NPC(const ComponentDesc& desc)
	: Component{ desc }
	, m_name{}
	, m_eventName{}
	, m_focusDistance{}
	, m_baseRotation{}
	, m_rotation{}
	, m_spinAngle{}
	, m_refTransform{ GetOwner()->GetNullReference<Transform>() }
	, m_refBoxCollider{ GetOwner()->GetNullReference<Colliders::BoxCollider>() }
	, m_refSphereCollider{ GetOwner()->GetNullReference<Colliders::SphereCollider>() }
{
}

// 初期化処理
void NPC::Initalize(const nlohmann::ordered_json& json, Ref<IGameObjectFinder> iGameObjectFinder)
{
	m_refTransform = GetOwner()->GetComponent<Transform>();
	m_refBoxCollider = GetOwner()->GetComponent<Colliders::BoxCollider>();
	m_refBoxCollider->ApplyTransform();
	m_refSphereCollider = GetOwner()->GetComponent<Colliders::SphereCollider>();
	m_refSphereCollider->ApplyTransform();

	m_baseRotation = m_refTransform->GetRotation();
	m_rotation.SetValue(m_baseRotation);
	m_spinAngle.SetMovement(0.0f, 0.0f);

	Systems::JsonSerializer serializer{ iGameObjectFinder };
	serializer.AddParameter(&m_name, "Name");
	serializer.AddParameter(&m_eventName, "EventName");
	serializer.AddParameter(&m_focusDistance, "FocusDistance");
	serializer.Load(json);
}

// 更新処理
void NPC::Update(float elapsedTime)
{
	if (m_spinAngle.IsMoving())
	{
		m_spinAngle.Tick(elapsedTime);
	}
	else
	{
		m_rotation.Tick(elapsedTime);
	}

	m_refTransform->SetRotation(m_rotation.GetCurrent() * Math::Euler{ 0.0f, m_spinAngle.GetMovement(), 0.0f }.CreateQuaternion());
	m_refBoxCollider->ApplyTransform();
	m_refSphereCollider->ApplyTransform();
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

// スピン
void NPC::Spin(bool isClockwise)
{
	m_spinAngle.SetMovement(0.0f, isClockwise ? 720.0f : -720.0f, 1.0f, Easing::Type::Back, Easing::InOut::Out);
}

// 中心座標を取得
const Math::Vector3& NPC::GetPosition() const
{
	return m_refBoxCollider->GetWorldBox().position;
}

// 当たり判定を取得
const Math::Box& NPC::GetBox() const
{
	return m_refBoxCollider->GetWorldBox();
}

// 当たり判定を取得
const Math::Sphere& NPC::GetSphere() const
{
	return m_refSphereCollider->GetWorldSphere();
}
