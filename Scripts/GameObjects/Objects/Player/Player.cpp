/*
 * FileName:     Player.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/09/28
 * 
 *
 * プレイヤー
 */

#include "Pch.h"
#include "Player.h"

#include "PlayerModel.h"
#include "Scripts/Commons/GameObjects/GameObject.h"
#include "Scripts/Commons/GameObjects/IGameObjectFinder.h"
#include "Scripts/Commons/Renderings/Model3D/Model3D.h"
#include "Scripts/Commons/Components/Transform.h"
#include "Scripts/Commons/Colliders/BoxCollider.h"
#include "Scripts/Commons/Systems/JsonSerializer.h"

// コンストラクタ
Player::Player(const ComponentDesc& desc)
	: Component{ desc }
	, m_params{}
	, m_moveVelocity{}
	, m_fallSpeed{}
	, m_moveMaxSpeed{}
	, m_rotation{}
	, m_fallState{}
	, m_fallCoyoteTime{}
	, m_jumpBufferTime{}
	, m_pTransform{ GetPOwner()->GetNullReferences<Transform>() }
	, m_pBoxCollider{ GetPOwner()->GetNullReferences<Colliders::BoxCollider>() }
	, m_pCameraScreen{ GetPOwner()->GetNullReferences<Renderings::CameraScreen<Camera::EulerTargetCamera>>() }
	, m_pModel{ GetPOwner()->GetNullReferences<PlayerModel>() }
{
}

// 初期化処理
void Player::Initalize(const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder)
{
	m_pTransform = GetPOwner()->GetComponent<Transform>();
	m_rotation.SetValue(m_pTransform->GetRotation());

	m_pBoxCollider = GetPOwner()->GetComponent<Colliders::BoxCollider>();

	float moveMaxSpeed = m_moveMaxSpeed.GetMin();
	float fallMaxSpeed = m_fallSpeed.GetMax();
	float jumpPower = -m_fallSpeed.GetMin();
	float coyoteTime = m_fallCoyoteTime.GetMax();

	Systems::JsonSerializer serializer{ pIGameObjectFinder };
	serializer.AddParameter(&moveMaxSpeed, "MoveMaxSpeed");
	serializer.AddParameter(&m_params.moveAcceleration, "MoveAcceleration");
	serializer.AddParameter(&m_params.dashRatio, "DashRatio");
	serializer.AddParameter(&fallMaxSpeed, "FallMaxSpeed");
	serializer.AddParameter(&m_params.fallAcceleration, "FallAcceleration");
	serializer.AddParameter(&jumpPower, "JumpPower");
	serializer.AddParameter(&m_params.cameraVelocityCoefficient, "CameraVelocityCoefficient");
	serializer.AddParameter(&coyoteTime, "CoyoteTime");
	serializer.AddParameter(&m_pCameraScreen, "CameraScreen");
	serializer.Load(json);

	m_moveMaxSpeed.Initialize(0.0f, moveMaxSpeed, moveMaxSpeed * m_params.dashRatio);

	m_fallSpeed.Initialize(0.0f, -jumpPower, fallMaxSpeed);

	m_fallState = FallState::OnGround;
	m_fallCoyoteTime.Initialize(0.0f, 0.0f, coyoteTime);
	m_jumpBufferTime.Initialize(0.0f, 0.0f, coyoteTime);

	// 当たり判定の更新
	m_pBoxCollider->ApplyTransform();

	// モデルの取得
	if (m_pModel == GetPOwner()->GetNullReferences<PlayerModel>())
	{
		GameObject* pObj = Instantiate("Prefab_PlayerModel");
		m_pModel = pObj->GetComponent<PlayerModel>();
	}

	// カメラを設定
	if (m_pCameraScreen != GetPOwner()->GetNullReferences<Renderings::CameraScreen<Camera::EulerTargetCamera>>())
	{
		m_pBoxCollider->AddICameraScreen(*m_pCameraScreen);
		m_pModel->GetPOwner()->GetComponent<Renderings::Model3D>()->AddICameraScreen(*m_pCameraScreen);
	}
}

// 更新処理
void Player::Update(float elapsedTime, const Math::Vector3& move, bool isDash, bool isJump)
{
	// カメラのY軸回転
	Math::Quaternion cameraRotation = Math::Quaternion::CreateFromAxisAngle(Math::Vector3::UnitY, Math::Deg2Rad(m_pCameraScreen->GetCamera().rotation.y));

	// 最大の早さを調整
	m_moveMaxSpeed += m_params.moveAcceleration * elapsedTime * m_params.dashRatio * (isDash ? 1.0f : -1.0f);

	// 加速度がない
	if (move == Math::Vector3::Zero)
	{
		// 速さ
		Limited speed = Limited::Create(m_moveVelocity.Length() - m_params.moveAcceleration * elapsedTime, 0.0f, m_moveMaxSpeed);
		m_moveVelocity.Normalize();
		m_moveVelocity *= speed;
	}
	// 加速度がある
	else
	{
		// 速度に加速度を足す
		m_moveVelocity += Math::Vector3::Transform(move, cameraRotation) * m_params.moveAcceleration * elapsedTime * (isDash ? m_params.dashRatio : 1.0f);
		// 速さを最大に収める
		if (m_moveVelocity.Length() > m_moveMaxSpeed)
		{
			m_moveVelocity.Normalize();
			m_moveVelocity *= m_moveMaxSpeed;
		}

		m_rotation.SetTarget
		(
			Math::Quaternion::CreateFromAxisAngle(Math::Vector3::UnitY, Math::Arctan2(m_moveVelocity.x, m_moveVelocity.z)),
			200.0f,
			25.0f
		);
	}

	m_fallSpeed += m_params.fallAcceleration * elapsedTime;

	// ジャンプ入力
	if (isJump)
	{
		m_jumpBufferTime = m_jumpBufferTime.GetMax();
	}

	m_jumpBufferTime -= elapsedTime;

	// ジャンプ
	if (m_jumpBufferTime != 0.0f && m_fallState != FallState::Falling)
	{
		m_fallState = FallState::Falling;
		m_fallSpeed = m_fallSpeed.GetMin();
	}

	// 回転の更新
	m_rotation.Tick(elapsedTime);

	// 位置に速度を足す
	m_pTransform->Translate((m_moveVelocity - Math::Vector3::UnitY * m_fallSpeed) * elapsedTime);
	// 回転させる
	m_pTransform->SetRotation(m_rotation.GetCurrent());

	// 落下猶予
	m_fallCoyoteTime -= elapsedTime;

	// 当たり判定の更新
	m_pBoxCollider->ApplyTransform();
}

// 直方体による座標補正
void Player::BoxCorrect(const std::vector<const Math::Box*>& pBoxes)
{
	// 直方体
	Math::Box box = m_pBoxCollider->GetWorldBox();

	for (const auto* pBox : pBoxes)
	{
		// 外接する非回転直方体同士が触れていなければ何もしない
		if (!Math::Geometry::IsCollide(box.CreateNonRotatingBox(), pBox->CreateNonRotatingBox()))
		{
			continue;
		}

		// 距離の方向
		Math::Vector3 direction{};
		// 距離
		float distance = Math::Geometry::Distance(box, *pBox, &direction);
		// めり込んでいるなら
		if (distance < 0.0f)
		{
			m_pTransform->Translate(direction * distance);
			box.position += direction * distance;

			// 押出方向が上かつ落下中のとき
			if (direction.Dot(Math::Vector3::Down) > 1.0f / 1.41421356f && m_fallSpeed > 0.0f)
			{
				m_fallSpeed = 0.0f;
				m_fallState = FallState::OnGround;
				m_fallCoyoteTime = m_fallCoyoteTime.GetMax();
			}
		}
	}

	// 当たり判定の更新
	m_pBoxCollider->ApplyTransform();
}

// メッシュによる座標補正
void Player::MeshCorrect(const Mesh& mesh)
{
	// 直方体
	Math::Box box = m_pBoxCollider->GetWorldBox();
	// 外接する非回転直方体（少し広めに）
	Math::NonRotatingBox nonRotatingBox = m_pBoxCollider->GetWorldBox().CreateNonRotatingBox();
	nonRotatingBox.size += Math::Vector3::One;

	// 壁リスト
	std::vector<Math::Triangle> walls{};
	// 床リスト
	std::vector<Math::Triangle> floors{};

	for (const auto& face : mesh.f)
	{
		// メッシュの三角形を取得
		Math::Triangle triangle = Math::Triangle{ mesh.v.at(face.at(0)), mesh.v.at(face.at(1)) , mesh.v.at(face.at(2)) };
		// 外接する非回転直方体が触れていなければ何もしない
		if (!Math::Geometry::IsCollide(nonRotatingBox, triangle.CreateNonRotatingBox()))
		{
			continue;
		}

		// 傾斜が 45° 以上なら
		if (Math::Abs(triangle.GetNormal().Dot(Math::Vector3::Down)) < 1.0f / 1.41421356f)
		{
			// 壁
			walls.push_back(triangle);
		}
		else
		{
			// 床
			floors.push_back(triangle);
		}
	}

	// 最短距離
	float distance = Math::INFTY;
	// 押出方向
	Math::Vector3 direction{};

	// 壁
	for (const auto& wall : walls)
	{
		Math::Vector3 newDirection{};

		float newDistance = Math::Geometry::Distance(wall, box, &newDirection);

		// 最短距離を更新
		if (newDistance < distance)
		{
			distance = newDistance;
			direction = newDirection;
		}
	}

	// めり込んでいるなら
	if (distance < 0.0f)
	{
		m_pTransform->Translate(direction * distance);
		box.position += direction * distance;
	}

	distance = Math::INFTY;
	direction = Math::Vector3::Zero;

	// 床
	for (const auto& floor : floors)
	{
		Math::Vector3 newDirection{};

		float newDistance = Math::Geometry::Distance(floor, box, &newDirection);

		float arccos = newDirection.Dot(Math::Vector3::UnitY);

		// 押出方向の傾斜が 45° 以上なら何もしない
		if (Math::Abs(arccos) < 1.0f / 1.41421356f)
		{
			continue;
		}

		newDistance /= Math::Abs(arccos);

		// 最短距離を更新
		if (newDistance < distance)
		{
			distance = newDistance;
			direction = newDirection;
		}
	}

	// 落下状態ごとの処理
	switch (m_fallState)
	{
	case Player::FallState::OnGround:
		// 地面に貼り付ける
		if (distance < box.size.y / 8.0f)
		{
			float arccos = direction.Dot(Math::Vector3::UnitY);

			if (distance < 0.0f || Math::Sign(arccos) == -1)
			{
				m_pTransform->Translate(Math::Vector3::UnitY * (Math::Sign(arccos) * distance));
				box.position += Math::Vector3::UnitY * (Math::Sign(arccos) * distance);
				m_fallSpeed = 0.0f;
				m_fallCoyoteTime = m_fallCoyoteTime.GetMax();
				break;
			}
		}
		m_fallState = FallState::OnAir;
		break;
	case Player::FallState::OnAir:
		if (distance < 0.0f)
		{
			float arccos = direction.Dot(Math::Vector3::UnitY);

			m_pTransform->Translate(Math::Vector3::UnitY * (Math::Sign(arccos) * distance));
			box.position += Math::Vector3::UnitY * (Math::Sign(arccos) * distance);
			m_fallSpeed = 0.0f;
			m_fallState = FallState::OnGround;
			m_fallCoyoteTime = m_fallCoyoteTime.GetMax();
			break;
		}
		if (m_fallCoyoteTime.IsMin())
		{
			m_fallState = FallState::Falling;
		}
		break;
	case Player::FallState::Falling:
		if (distance < 0.0f)
		{
			float arccos = direction.Dot(Math::Vector3::UnitY);

			m_pTransform->Translate(Math::Vector3::UnitY * (Math::Sign(arccos) * distance));
			box.position += Math::Vector3::UnitY * (Math::Sign(arccos) * distance);
			// 落下中のとき
			if (m_fallSpeed > 0.0f)
			{
				m_fallSpeed = 0.0f;
				m_fallState = FallState::OnGround;
				m_fallCoyoteTime = m_fallCoyoteTime.GetMax();
			}
			break;
		}
		break;
	default:
		break;
	}

	// 当たり判定の更新
	m_pBoxCollider->ApplyTransform();
}

// モデルの更新
void Player::UpdateModel(float elapsedTime)
{
	m_pModel->SetTransform(*m_pTransform);
	m_pModel->Update(elapsedTime, *this);
}

// 中心座標を取得
const Math::Vector3& Player::GetPosition() const
{
	return m_pBoxCollider->GetWorldBox().position;
}

// 当たり判定を取得
const Math::Box& Player::GetBox() const
{
	return m_pBoxCollider->GetWorldBox();
}

// 移動速度の比率を取得
float Player::GetMoveVelocityRatio() const
{
	return m_moveVelocity.Length() / m_moveMaxSpeed.GetMin();
}

// カメラの目標座標を取得
Math::Vector3 Player::GetCameraTarget() const
{
	return GetPosition() + (m_moveVelocity + m_fallSpeed * Math::Vector3::Down) * m_params.cameraVelocityCoefficient;
}
