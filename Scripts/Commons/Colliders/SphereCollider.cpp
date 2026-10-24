/*
 * FileName:     SphereCollider.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/24
 *
 * 球の当たり判定
 */

#include "Pch.h"
#include "SphereCollider.h"

#include "../GameObjects/GameObject.h"
#include "../GameObjects/IGameObjectFinder.h"
#include "../Components/Transform.h"
#include "../Renderings/ColliderRenderer.h"
#include "../Renderings/ICameraScreen.h"
#include "../Systems/JsonSerializer.h"
#include "../Systems/IsDerivedRegister.h"

// コンストラクタ
Colliders::SphereCollider::SphereCollider(const ComponentDesc& desc, Ref<Renderings::IColliderRenderer> iColliderRendererRef)
	: Component{ desc }
	, m_sphere{}
	, m_worldSphere{}
	, m_color{ DirectX::Colors::LightGreen }
	, m_refTransform{ GetOwner()->GetNullComponent<Transform>() }
	, m_iCameraScreenRefs{}
	, m_iColliderRendererRef{ iColliderRendererRef }
{
	// 描画者に球を追加
	m_iColliderRendererRef->AddPSphereCollider(this);
}

// デストラクタ
Colliders::SphereCollider::~SphereCollider()
{
	// 描画者から長方形を削除
	m_iColliderRendererRef->RemovePSphereCollider(this);
}

// 初期化処理
void Colliders::SphereCollider::Initalize(const nlohmann::ordered_json& json, Ref<IGameObjectFinder> iGameObjectFinderRef)
{
	m_refTransform = GetOwner()->GetComponent<Transform>();

	Systems::JsonSerializer serializer{ iGameObjectFinderRef };
	serializer.AddParameter(&m_sphere.centerPosition, "Position");
	serializer.AddParameter(&m_sphere.radius, "Radius");
	serializer.AddParameter(&m_color, "Color");
	serializer.AddParameter(&m_iCameraScreenRefs, "CameraScreens");
	serializer.Load(json);
}

// 映るカメラ画面を追加
void Colliders::SphereCollider::AddICameraScreenRef(Ref<const Renderings::ICameraScreen> iCameraScreenRef)
{
	if (std::ranges::find(m_iCameraScreenRefs, iCameraScreenRef) == m_iCameraScreenRefs.end())
	{
		m_iCameraScreenRefs.push_back(iCameraScreenRef);
	}
}

// 映るカメラ画面を削除
void Colliders::SphereCollider::RemoveICameraScreenRef(Ref<const Renderings::ICameraScreen> iCameraScreenRef)
{
	auto it = std::ranges::find(m_iCameraScreenRefs, iCameraScreenRef);
	if (it != m_iCameraScreenRefs.end())
	{
		m_iCameraScreenRefs.erase(it);
	}
}

// トランスフォームを適用
void Colliders::SphereCollider::ApplyTransform()
{
	// 取得できなければ何もしない
	if (m_refTransform == GetOwner()->GetNullComponent<Transform>())
	{
		m_worldSphere = m_sphere;
		return;
	}

	// ワールド行列
	Math::Matrix world = m_refTransform->GetWorldMatrix();
	// 拡大
	Math::Vector3 scale = m_refTransform->GetScale();

	m_worldSphere.centerPosition = Math::Vector3::Transform(m_sphere.centerPosition, world);
	m_worldSphere.radius = m_sphere.radius * Math::Max(scale.x, scale.y, scale.z);
}
