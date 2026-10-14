/*
 * FileName:     TargetPoint.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/14
 *
 * ターゲット照準
 */

#include "Pch.h"
#include "TargetPoint.h"

#include "Scripts/Commons/Components/RectTransform.h"
#include "Scripts/Commons/Colliders/SphereCollider.h"
#include "Scripts/Commons/GameObjects/GameObject.h"
#include "Scripts/Commons/Renderings/ICameraScreen.h"
#include "Scripts/Commons/Renderings/Image/Image.h"
#include "Scripts/Commons/Renderings/Canvas.h"
#include "Scripts/Commons/Systems/JsonSerializer.h"

// コンストラクタ
TargetPoint::TargetPoint(const ComponentDesc& desc)
	: Component{ desc }
	, m_targetDistance{}
	, m_brightInterval{}
	, m_brightTimer{}
	, m_pTransform{ GetPOwner()->GetNullReferences<RectTransform>() }
	, m_pImage{ GetPOwner()->GetNullReferences<Renderings::Image>() }
	, m_pTargets{}
	, m_pTarget{}
{
}

// 初期化処理
void TargetPoint::Initalize(const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder)
{
	m_pImage = GetPOwner()->GetComponent<Renderings::Image>();
	m_pTransform = GetPOwner()->GetComponent<RectTransform>();

	Systems::JsonSerializer serializer{ pIGameObjectFinder };
	serializer.AddParameter(&m_targetDistance, "TargetDistance");
	serializer.AddParameter(&m_brightInterval, "BrightInterval");
	serializer.AddParameter(&m_pTargets, "Targets");
	serializer.Load(json);
}

// 更新処理
void TargetPoint::Update(float elapsedTime)
{
	// 点滅
	m_brightTimer += elapsedTime;
	if (m_brightTimer >= m_brightInterval)
	{
		m_brightTimer -= m_brightInterval;
		Math::Color color = Math::Color{ m_pImage->GetColor() == Math::Color{ DirectX::Colors::Red } ? DirectX::Colors::Orange : DirectX::Colors::Red };
		m_pImage->SetColor(color);
	}
}

// ターゲットを設定
void TargetPoint::SetTarget(const Math::Vector3& playerPosition, const Renderings::ICameraScreen& iCameraScreen)
{
	// 距離の最小
	float distanceMin = 0.0f;

	m_pTarget = nullptr;

	for (auto* pTarget : m_pTargets)
	{
		// 球
		const Math::Sphere& sphere = pTarget->GetWorldSphere();

		// カメラの中心から発射されるレイとの距離
		float distance = Math::Geometry::Distance
		(
			sphere,
			Math::Line{ iCameraScreen.GetEyePosition(), iCameraScreen.GetEyePosition() + iCameraScreen.GetEyeDirection() * 100.0f }
		);

		// 最も距離が短いターゲットを記録
		if (distance < distanceMin)
		{
			float playerDistance = Math::Geometry::Distance(playerPosition, sphere);
			if (playerDistance >= 0.0f && playerDistance <= m_targetDistance)
			{
				distanceMin = distance;
				m_pTarget = pTarget;
			}
		}
	}

	if (m_pTarget)
	{
		Math::Vector2 screenPosition = Math::Geometry::Projection
		(
			m_pTarget->GetWorldSphere().centerPosition,
			iCameraScreen.GetViewMatrix(),
			iCameraScreen.GetProjectionMatrix(),
			m_pImage->GetPCanvas()->GetSize()
		);

		m_pTransform->SetPosition(screenPosition);
	}
	else
	{
		m_pTransform->SetPosition(Math::Vector2{ -10.0f, -10.0f });
	}
}

// ターゲットを表示
void TargetPoint::Appear(bool isAppear)
{
	m_pImage->SetColor(isAppear ? DirectX::Colors::Red : DirectX::Colors::Transparent);
}
