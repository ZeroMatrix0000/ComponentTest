/*
 * FileName:     MeshCollider.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/24
 *
 * メッシュの当たり判定
 */

#include "Pch.h"
#include "MeshCollider.h"

#include "../GameObjects/GameObject.h"
#include "../GameObjects/IGameObjectFinder.h"
#include "../Components/Transform.h"
#include "../Renderings/ColliderRenderer.h"
#include "../Systems/IResources.h"
#include "../Systems/JsonSerializer.h"
#include "../Systems/IsDerivedRegister.h"

// コンストラクタ
Colliders::MeshCollider::MeshCollider
(
	const ComponentDesc& desc,
	Ref<Renderings::IColliderRenderer> iColliderRendererRef,
	Ref<Systems::IResources> iResourcesRef
)
	: Component{ desc }
	, m_color{ DirectX::Colors::LightGreen }
	, m_worldMesh{}
	, m_meshRef{ iResourcesRef->GetNullMesh() }
	, m_transformRef{ GetOwner()->GetNullComponent<Transform>() }
	, m_iCameraScreenRefs{}
	, m_iColliderRendererRef{ iColliderRendererRef }
	, m_iResourcesRef{ iResourcesRef }
{
	// 描画者に長方形を追加
	m_iColliderRendererRef->AddPMeshCollider(this);
}

// デストラクタ
Colliders::MeshCollider::~MeshCollider()
{
	// 描画者から長方形を削除
	m_iColliderRendererRef->RemovePMeshCollider(this);
}

// 初期化処理
void Colliders::MeshCollider::Initalize(const nlohmann::ordered_json& json, Ref<IGameObjectFinder> iGameObjectFinderRef)
{
	m_transformRef = GetOwner()->GetComponent<Transform>();

	std::string meshName{};

	Systems::JsonSerializer serializer{ iGameObjectFinderRef };
	serializer.AddParameter(&meshName, "MeshName");
	serializer.AddParameter(&m_color, "Color");
	serializer.AddParameter(&m_iCameraScreenRefs, "CameraScreens");
	serializer.Load(json);

	if (!meshName.empty())
	{
		SetMesh(meshName);
	}
}

// メッシュを設定
void Colliders::MeshCollider::SetMesh(const std::string& meshName)
{
	m_meshRef = m_iResourcesRef->GetMesh(meshName);
}

// 映るカメラ画面を追加
void Colliders::MeshCollider::AddICameraScreenRef(Ref<const Renderings::ICameraScreen> iCameraScreenRef)
{
	if (std::ranges::find(m_iCameraScreenRefs, iCameraScreenRef) == m_iCameraScreenRefs.end())
	{
		m_iCameraScreenRefs.push_back(iCameraScreenRef);
	}
}

// 映るカメラ画面を削除
void Colliders::MeshCollider::RemoveICameraScreenRef(Ref<const Renderings::ICameraScreen> iCameraScreenRef)
{
	auto it = std::ranges::find(m_iCameraScreenRefs, iCameraScreenRef);
	if (it != m_iCameraScreenRefs.end())
	{
		m_iCameraScreenRefs.erase(it);
	}
}

// トランスフォームを適用
void Colliders::MeshCollider::ApplyTransform()
{
	m_worldMesh = m_meshRef.Get();
	m_worldMesh.ApplyMatrix(m_transformRef->GetWorldMatrix());
}
