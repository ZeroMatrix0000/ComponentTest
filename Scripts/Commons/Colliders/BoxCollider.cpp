/*
 * FileName:     BoxCollider.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/24
 *
 * 長方形の当たり判定
 */

#include "Pch.h"
#include "BoxCollider.h"

#include "../GameObjects/GameObject.h"
#include "../GameObjects/IGameObjectFinder.h"
#include "../Components/Transform.h"
#include "../Renderings/ColliderRenderer.h"
#include "../Systems/JsonSerializer.h"
#include "../Systems/IsDerivedRegister.h"

// コンストラクタ
Colliders::BoxCollider::BoxCollider(const ComponentDesc& desc, Ref<Renderings::IColliderRenderer> iColliderRendererRef)
	: Component{ desc }
	, m_box{}
	, m_worldBox{}
	, m_color{ DirectX::Colors::LightGreen }
	, m_transformRef{ GetOwner()->GetNullComponent<Transform>() }
	, m_iCameraScreenRefs{}
	, m_iColliderRendererRef{ iColliderRendererRef }
{
	// 描画者に長方形を追加
	m_iColliderRendererRef->AddPBoxCollider(this);
}

// デストラクタ
Colliders::BoxCollider::~BoxCollider()
{
	// 描画者から長方形を削除
	m_iColliderRendererRef->RemovePBoxCollider(this);
}

// 初期化処理
void Colliders::BoxCollider::Initalize(const nlohmann::ordered_json& json, Ref<IGameObjectFinder> iGameObjectFinderRef)
{
	m_transformRef = GetOwner()->GetComponent<Transform>();

	Systems::JsonSerializer serializer{ iGameObjectFinderRef };
	serializer.AddParameter(&m_box.position, "Position");
	serializer.AddParameter(&m_box.size, "Size");
	serializer.AddParameter(&m_color, "Color");
	serializer.AddParameter(&m_iCameraScreenRefs, "CameraScreens");
	serializer.Load(json);
}

// 映るカメラ画面を追加
void Colliders::BoxCollider::AddICameraScreenRef(Ref<const Renderings::ICameraScreen> iCameraScreenRef)
{
	if (std::ranges::find(m_iCameraScreenRefs, iCameraScreenRef) == m_iCameraScreenRefs.end())
	{
		m_iCameraScreenRefs.push_back(iCameraScreenRef);
	}
}

// 映るカメラ画面を削除
void Colliders::BoxCollider::RemoveICameraScreenRef(Ref<const Renderings::ICameraScreen> iCameraScreenRef)
{
	auto it = std::ranges::find(m_iCameraScreenRefs, iCameraScreenRef);
	if (it != m_iCameraScreenRefs.end())
	{
		m_iCameraScreenRefs.erase(it);
	}
}

// トランスフォームを適用
void Colliders::BoxCollider::ApplyTransform()
{
	// ワールド行列
	Math::Matrix world = m_transformRef->GetWorldMatrix();

	m_worldBox.position = Math::Vector3::Transform(m_box.position, world);
	m_worldBox.size = m_box.size * m_transformRef->GetScale();
	m_worldBox.rotation = m_transformRef->GetRotation();
}
