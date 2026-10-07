/*
 * FileName:     BoxCollider.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/07
 *
 * 長方形の当たり判定
 */

#include "Pch.h"
#include "BoxCollider.h"

#include "../GameObjects/GameObject.h"
#include "../GameObjects/IGameObjectFinder.h"
#include "../Components/Transform.h"
#include "../Renderings/ColliderRenderer.h"
#include "../Renderings/ICameraScreen.h"
#include "../Systems/JsonSerializer.h"

// コンストラクタ
Colliders::BoxCollider::BoxCollider(const ComponentDesc& desc, Renderings::IColliderRenderer* pIColliderRenderer)
	: Component{ desc }
	, m_box{}
	, m_worldBox{}
	, m_color{ DirectX::Colors::LightGreen }
	, m_pTransform{ GetPOwner()->GetNullReferences<Transform>() }
	, m_pICameraScreens{}
	, m_pIColliderRenderer{ pIColliderRenderer }
{
	// 描画者に長方形を追加
	m_pIColliderRenderer->AddPBoxCollider(this);
}

// デストラクタ
Colliders::BoxCollider::~BoxCollider()
{
	// 描画者から長方形を削除
	m_pIColliderRenderer->RemovePBoxCollider(this);
}

// 初期化処理
void Colliders::BoxCollider::Initalize(const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder)
{
	m_pTransform = GetPOwner()->GetComponent<Transform>();

	Systems::JsonSerializer serializer{ pIGameObjectFinder };
	serializer.AddParameter(&m_box.position, "Position");
	serializer.AddParameter(&m_box.size, "Size");
	serializer.AddParameter(&m_color, "Color");
	serializer.AddParameter(&m_pICameraScreens, "CameraScreens");
	serializer.Load(json);
}

// 映るカメラ画面を追加
void Colliders::BoxCollider::AddICameraScreen(const Renderings::ICameraScreen& iCameraScreen)
{
	if (std::ranges::find(m_pICameraScreens, &iCameraScreen) == m_pICameraScreens.end())
	{
		m_pICameraScreens.push_back(&iCameraScreen);
	}
}

// 映るカメラ画面を削除
void Colliders::BoxCollider::RemoveICameraScreen(const Renderings::ICameraScreen& iCameraScreen)
{
	auto it = std::ranges::find(m_pICameraScreens, &iCameraScreen);
	if(it != m_pICameraScreens.end())
	{
		m_pICameraScreens.erase(it);
	}
}

// トランスフォームを適用
void Colliders::BoxCollider::ApplyTransform()
{
	// ワールド行列
	Math::Matrix world = m_pTransform->GetWorldMatrix();

	m_worldBox.position = Math::Vector3::Transform(m_box.position, world);
	m_worldBox.size = m_box.size * m_pTransform->GetScale();
	m_worldBox.rotation = m_pTransform->GetRotation();
}
