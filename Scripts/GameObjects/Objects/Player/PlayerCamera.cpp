/*
 * FileName:     PlayerCamera.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/21
 *
 * プレイヤーカメラ
 */

#include "Pch.h"
#include "PlayerCamera.h"

#include "Scripts/Commons/GameObjects/GameObject.h"

// コンストラクタ
PlayerCamera::PlayerCamera(const ComponentDesc& desc)
	: Component{ desc }
	, m_target{}
	, m_rotationYDeg{}
	, m_rotationXDeg{}
	, m_distance{}
	, m_maxDistance{}
	, m_camera{}
	, m_refCameraScreen{ GetOwner()->GetNullReference<Renderings::CameraScreen<Camera::EulerTargetCamera>>() }
{
}

// 初期化処理
void PlayerCamera::Initalize(const nlohmann::ordered_json& json, Ref<IGameObjectFinder> iGameObjectFinder)
{
	// カメラ画面を取得
	m_refCameraScreen = GetOwner()->GetComponent<Renderings::CameraScreen<Camera::EulerTargetCamera>>();
	// カメラ
	const auto& camera = m_refCameraScreen->GetCamera();

	m_target = camera.position;
	m_rotationYDeg.Initialize(camera.rotation.y, 0.0f, 360.0f);
	m_rotationXDeg.Initialize(camera.rotation.x, -80.0f, 45.0f);
	m_distance = camera.distance;
	m_maxDistance = camera.distance;

	m_camera.SetValue(camera);
}

// 更新処理
void PlayerCamera::Update(float elapsedTime)
{
	// カメラを更新
	//m_camera.SetValue(Camera::EulerTargetCamera{ m_target, Math::Euler{ m_rotationXDeg, m_rotationYDeg, 0.0f }, m_distance });
	m_camera.SetTarget(Camera::EulerTargetCamera{ m_target, Math::Euler{ m_rotationXDeg, m_rotationYDeg, 0.0f }, m_distance }, 250.0f, 30.0f);
	m_camera.Tick(elapsedTime);
	m_refCameraScreen->SetCamera(m_camera.GetCurrent());
}

// 回転
void PlayerCamera::Rotate(const Math::Vector2& rotate)
{
	// 回転量
	m_rotationYDeg -= rotate.x;
	m_rotationXDeg -= rotate.y;
}

// メッシュによる座標補正
void PlayerCamera::MeshCorrect(const Mesh& mesh, const Math::Vector3& centerPosition)
{
	// レイの方向
	Math::Vector3 direction = Math::Vector3::Transform(Math::Vector3::UnitZ, Math::Euler{ m_rotationXDeg, m_rotationYDeg, 0.0f }.CreateQuaternion());
	
	// 方向指定最短距離
	float angledDistance = Math::INFTY;

	for (const auto& face : mesh.f)
	{
		// メッシュの三角形を取得
		Math::Triangle triangle = Math::Triangle{ mesh.v.at(face.at(0)), mesh.v.at(face.at(1)), mesh.v.at(face.at(2)) };
		
		float newDistance = Math::Geometry::AngledDistance(m_target, triangle, direction);

		if (newDistance > 0.0f && direction.Dot(triangle.GetNormal()) < 0.0f)
		{
			angledDistance = Math::Min(angledDistance, newDistance);
		}
	}

	m_distance = Math::Min(angledDistance, m_maxDistance);
}

// 直方体による座標補正
void PlayerCamera::BoxCorrect(const Math::Box& box, const Math::Vector3& centerPosition)
{
	// レイの方向
	Math::Vector3 direction = Math::Vector3::Transform(Math::Vector3::UnitZ, Math::Euler{ m_rotationXDeg, m_rotationYDeg, 0.0f }.CreateQuaternion());

	// 方向指定最短距離
	float angledDistance = Math::Geometry::AngledDistance(m_target, box, direction);
	if (angledDistance <= 0.0f)
	{
		angledDistance = Math::INFTY;
	}

	m_distance = Math::Min(angledDistance, m_distance);
}

// カメラを移動
void PlayerCamera::SetPosition(const Math::Vector3& position)
{
	Camera::EulerTargetCamera camera = m_camera.GetCurrent();
	camera.position = position;
	m_camera.SetValue(camera);

	m_refCameraScreen->SetCamera(m_camera.GetCurrent());
}
