/*
 * FileName:     TargetPoint.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/15
 *
 * ターゲット照準
 */

#include "Pch.h"
#include "TargetPoint.h"

#include "Scripts/Commons/Components/RectTransform.h"
#include "Scripts/Commons/Colliders/BoxCollider.h"
#include "Scripts/Commons/Colliders/SphereCollider.h"
#include "Scripts/Commons/Colliders/MeshCollider.h"
#include "Scripts/Commons/GameObjects/GameObject.h"
#include "Scripts/Commons/Renderings/ICameraScreen.h"
#include "Scripts/Commons/Renderings/Image/Image.h"
#include "Scripts/Commons/Renderings/Canvas.h"
#include "Scripts/Commons/Systems/JsonSerializer.h"

// コンストラクタ
TargetPoint::TargetPoint(const ComponentDesc& desc)
	: Component{ desc }
	, m_targetDistance{}
	, m_rayLength{}
	, m_brightInterval{}
	, m_brightTimer{}
	, m_pTransform{ GetPOwner()->GetNullReferences<RectTransform>() }
	, m_pImage{ GetPOwner()->GetNullReferences<Renderings::Image>() }
	, m_pPlayer{ GetPOwner()->GetNullReferences<Colliders::BoxCollider>() }
	, m_pICameraScreen{ GetPOwner()->GetNullReferences<Renderings::ICameraScreen>() }
	, m_pTargetObjects{}
	, m_pGrounds{}
	, m_pTargetObject{}
{
}

// 初期化処理
void TargetPoint::Initalize(const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder)
{
	m_pImage = GetPOwner()->GetComponent<Renderings::Image>();
	m_pTransform = GetPOwner()->GetComponent<RectTransform>();

	Systems::JsonSerializer serializer{ pIGameObjectFinder };
	serializer.AddParameter(&m_targetDistance, "TargetDistance");
	serializer.AddParameter(&m_rayLength, "RayLength");
	serializer.AddParameter(&m_brightInterval, "BrightInterval");
	serializer.AddParameter(&m_pPlayer, "Player");
	serializer.AddParameter(&m_pICameraScreen, "CameraScreen");
	serializer.AddParameter(&m_pTargetObjects, "TargetObjects");
	serializer.AddParameter(&m_pGrounds, "Grounds");
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

	// 画面の中心に最も近いターゲットを探す
	{
		// 距離の最小
		float distanceMin = 0.0f;

		// プレイヤーの座標
		const Math::Vector3& playerPosition = m_pPlayer->GetWorldBox().position;

		// カメラの目線の位置
		Math::Vector3 eyePosition = m_pICameraScreen->GetEyePosition();
		// カメラの目線の方向
		Math::Vector3 eyeDirection = m_pICameraScreen->GetEyeDirection();

		m_pTargetObject = nullptr;

		for (auto* pTargetObject : m_pTargetObjects)
		{
			// 球
			const Math::Sphere& sphere = pTargetObject->GetWorldSphere();

			// カメラの中心から発射されるレイとの距離
			float distance = Math::Geometry::Distance
			(
				sphere,
				Math::Line{ eyePosition, eyePosition + eyeDirection * m_rayLength }
			);

			// 最も距離が短いターゲットを記録
			if (distance < distanceMin && eyeDirection.Dot(sphere.centerPosition - playerPosition) > 0.0f)
			{
				// プレイヤーとの距離
				float playerDistance = Math::Vector3::Distance(playerPosition, sphere.centerPosition);
				if (playerDistance >= 0.0f && playerDistance <= m_targetDistance)
				{
					distanceMin = distance;
					m_pTargetObject = pTargetObject;
				}
			}
		}
	}

	// 画像の位置を変更
	if (m_pTargetObject)
	{
		Math::Vector2 screenPosition = Math::Geometry::Projection
		(
			m_pTargetObject->GetWorldSphere().centerPosition,
			m_pICameraScreen->GetViewMatrix(),
			m_pICameraScreen->GetProjectionMatrix(),
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

// ターゲット情報を生成
TargetPoint::Info TargetPoint::CreateTargetInfo() const
{
	Info info{};

	if (m_pTargetObject)
	{
		info.position = m_pTargetObject->GetWorldSphere().centerPosition;
		info.type = Type::Object;
		return info;
	}

	// 距離の最小
	float distanceMin = m_targetDistance;

	// プレイヤーの座標
	const Math::Vector3& playerPosition = m_pPlayer->GetWorldBox().position;

	// カメラの目線の位置
	Math::Vector3 eyePosition = m_pICameraScreen->GetEyePosition();
	// カメラの目線の方向
	Math::Vector3 eyeDirection = m_pICameraScreen->GetEyeDirection();

	for (const auto* pGround : m_pGrounds)
	{
		const auto& mesh = pGround->GetWorldMesh();

		for (const auto& face : mesh.f)
		{
			// メッシュの三角形
			Math::Triangle triangle = Math::Triangle{ mesh.v.at(face.at(0)), mesh.v.at(face.at(1)) , mesh.v.at(face.at(2)) };

			// カメラの中心と三角形の方向指定最短距離
			float distance = Math::Geometry::AngledDistance(eyePosition, triangle, eyeDirection);
			if (distance == Math::INFTY)
			{
				continue;
			}

			// 交点
			Math::Vector3 intersect = eyePosition + eyeDirection * distance;

			// プレイヤーより手前側にあるなら
			if (eyeDirection.Dot(intersect - playerPosition) > 0.0f)
			{
				// プレイヤーとの距離
				float playerDistance = Math::Vector3::Distance(playerPosition, intersect);
				if (playerDistance <= m_targetDistance)
				{
					distanceMin = playerDistance;
					info.position = intersect;
				}
			}
		}
	}

	if (distanceMin != m_targetDistance)
	{
		info.type = Type::Ground;
		return info;
	}

	distanceMin = Math::Geometry::AngledDistance(eyePosition, Math::Sphere{ playerPosition, m_targetDistance }, eyeDirection).second;
	info.position = eyePosition + eyeDirection * distanceMin;
	info.type = Type::Air;
	return info;
}
