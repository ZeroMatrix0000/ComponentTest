/*
 * FileName:     SphereCollider.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/07
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

// コンストラクタ
Colliders::SphereCollider::SphereCollider(const ComponentDesc& desc, Renderings::IColliderRenderer* pIColliderRenderer)
	: Component{ desc }
	, m_sphere{}
	, m_worldSphere{}
	, m_color{ DirectX::Colors::LightGreen }
	, m_pTransform{ GetPOwner()->GetNullReferences<Transform>() }
	, m_pICameraScreens{}
	, m_pIColliderRenderer{ pIColliderRenderer }
{
	// 描画者に球を追加
	m_pIColliderRenderer->AddPSphereCollider(this);
}

// デストラクタ
Colliders::SphereCollider::~SphereCollider()
{
	// 描画者から長方形を削除
	m_pIColliderRenderer->RemovePSphereCollider(this);
}

// 初期化処理
void Colliders::SphereCollider::Initalize(const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder)
{
	m_pTransform = GetPOwner()->GetComponent<Transform>();

	Systems::JsonSerializer serializer{ pIGameObjectFinder };
	serializer.AddParameter(&m_sphere.centerPosition, "Position");
	serializer.AddParameter(&m_sphere.radius, "Radius");
	serializer.AddParameter(&m_color, "Color");
	serializer.AddParameter(&m_pICameraScreens, "CameraScreens");
	serializer.Load(json);
}

// 映るカメラ画面を追加
void Colliders::SphereCollider::AddICameraScreen(const Renderings::ICameraScreen& iCameraScreen)
{
	if (std::ranges::find(m_pICameraScreens, &iCameraScreen) == m_pICameraScreens.end())
	{
		m_pICameraScreens.push_back(&iCameraScreen);
	}
}

// 映るカメラ画面を削除
void Colliders::SphereCollider::RemoveICameraScreen(const Renderings::ICameraScreen& iCameraScreen)
{
	auto it = std::ranges::find(m_pICameraScreens, &iCameraScreen);
	if (it != m_pICameraScreens.end())
	{
		m_pICameraScreens.erase(it);
	}
}

// トランスフォームを適用
void Colliders::SphereCollider::ApplyTransform()
{
	// 取得できなければ何もしない
	if (m_pTransform == GetPOwner()->GetNullReferences<Transform>())
	{
		m_worldSphere = m_sphere;
		return;
	}

	// ワールド行列
	Math::Matrix world = m_pTransform->GetWorldMatrix();
	// 拡大
	Math::Vector3 scale = m_pTransform->GetScale();

	m_worldSphere.centerPosition = Math::Vector3::Transform(m_sphere.centerPosition, world);
	m_worldSphere.radius = m_sphere.radius * Math::Max(scale.x, scale.y, scale.z);
}
