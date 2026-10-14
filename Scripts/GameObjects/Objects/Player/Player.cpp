/*
 * FileName:     Player.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/14
 * 
 *
 * プレイヤー
 */

#include "Pch.h"
#include "Player.h"

#include "PlayerModel.h"
#include "States/PlayerStates.h"
#include "Scripts/Commons/GameObjects/GameObject.h"
#include "Scripts/Commons/GameObjects/IGameObjectFinder.h"
#include "Scripts/Commons/Renderings/Model3D/Model3D.h"
#include "Scripts/Commons/Renderings/Particle/ParticleManager.h"
#include "Scripts/Commons/Components/Transform.h"
#include "Scripts/Commons/Colliders/BoxCollider.h"
#include "Scripts/Commons/Systems/JsonSerializer.h"
#include "Scripts/GameObjects/Particles/LandingParticle.h"
#include "Scripts/GameObjects/Particles/JumpingParticle.h"

// コンストラクタ
Player::Player(const ComponentDesc& desc)
	: Component{ desc }
	, m_internals{}
{
	m_internals.P_CAMERA_SCREEN = GetPOwner()->GetNullReferences<Renderings::CameraScreen<Camera::EulerTargetCamera>>();
	m_internals.pTransform = GetPOwner()->GetNullReferences<Transform>();
	m_internals.pBoxCollider = GetPOwner()->GetNullReferences<Colliders::BoxCollider>();
	m_internals.pParticleManager = GetPOwner()->GetNullReferences<Renderings::ParticleManager>();
	m_internals.pModel = GetPOwner()->GetNullReferences<PlayerModel>();
}

// 初期化処理
void Player::Initalize(const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder)
{
	m_internals.pTransform = GetPOwner()->GetComponent<Transform>();
	m_internals.pBoxCollider = GetPOwner()->GetComponent<Colliders::BoxCollider>();
	m_internals.pParticleManager = GetPOwner()->GetComponent<Renderings::ParticleManager>();

	m_internals.rotation.SetValue(m_internals.pTransform->GetRotation());

	float moveMaxSpeed = m_internals.moveMaxSpeed.GetMin();
	float fallMaxSpeed = m_internals.fallSpeed.GetMax();
	float coyoteTime = m_internals.fallCoyoteTime.GetMax();

	Systems::JsonSerializer serializer{ pIGameObjectFinder };
	serializer.AddParameter(&moveMaxSpeed, "MoveMaxSpeed");
	serializer.AddParameter(&m_internals.MOVE_ACCELERATION, "MoveAcceleration");
	serializer.AddParameter(&m_internals.DASH_RATIO, "DashRatio");
	serializer.AddParameter(&fallMaxSpeed, "FallMaxSpeed");
	serializer.AddParameter(&m_internals.FALL_ACCELERATION, "FallAcceleration");
	serializer.AddParameter(&m_internals.JUMP_POWER, "JumpPower");
	serializer.AddParameter(&m_internals.CAMERA_VELOCITY_COEFFICIENT, "CameraVelocityCoefficient");
	serializer.AddParameter(&coyoteTime, "CoyoteTime");
	serializer.AddParameter(&m_internals.P_CAMERA_SCREEN, "CameraScreen");
	serializer.Load(json);

	m_internals.moveMaxSpeed.Initialize(0.0f, moveMaxSpeed, moveMaxSpeed * m_internals.DASH_RATIO);

	m_internals.fallSpeed.Initialize(0.0f, -m_internals.JUMP_POWER * 2.0f, fallMaxSpeed);

	m_internals.fallCoyoteTime.Initialize(0.0f, 0.0f, coyoteTime);
	m_internals.jumpBufferTime.Initialize(0.0f, 0.0f, coyoteTime);

	// 当たり判定の更新
	m_internals.pBoxCollider->ApplyTransform();

	// モデルの取得
	if (m_internals.pModel == GetPOwner()->GetNullReferences<PlayerModel>())
	{
		GameObject* pObj = Instantiate("Prefab_PlayerModel");
		m_internals.pModel = pObj->GetComponent<PlayerModel>();
		m_internals.pModel->SetParticleManager(m_internals.pParticleManager);
	}

	// カメラを設定
	if (m_internals.P_CAMERA_SCREEN != GetPOwner()->GetNullReferences<Renderings::CameraScreen<Camera::EulerTargetCamera>>())
	{
		m_internals.pBoxCollider->AddICameraScreen(*m_internals.P_CAMERA_SCREEN);
		m_internals.pParticleManager->AddICameraScreen(*m_internals.P_CAMERA_SCREEN);
		m_internals.pModel->AddICameraScreen(*m_internals.P_CAMERA_SCREEN);
	}

	// 現在の状態を初期化
	SetState(std::make_unique<PlayerStateOnGround>());
}

// 更新処理
void Player::Update(float elapsedTime, const Math::Vector3& move, bool isDash, bool isJump)
{
	// 入力を受け取る
	m_internals.MOVE = move;
	m_internals.IS_DASH = isDash;
	m_internals.IS_JUMP = isJump;

	// 状態ごとの処理
	m_currentState->Update(&m_internals, elapsedTime);
	// 次の状態に遷移
	std::unique_ptr<Systems::State<PlayerInternals>> nextState{ m_currentState->MoveNextState() };
	if (nextState)
	{
		PlayerState* pNextState = static_cast<PlayerState*>(nextState.release());
		SetState(std::unique_ptr<PlayerState>{ pNextState });
	}
}

// 座標補正
void Player::PositionCorrect(float distance, const Math::Vector3& direction)
{
	m_currentState->PositionCorrect(&m_internals, distance, direction);
}

// メッシュによる座標補正
void Player::MeshCorrect(const Mesh& mesh)
{
	// 直方体
	const Math::Box& box = GetBox();
	// 外接する非回転直方体（少し広めに）
	Math::NonRotatingBox nonRotatingBox = m_internals.pBoxCollider->GetWorldBox().CreateNonRotatingBox();
	nonRotatingBox.size *= 1.2f;

	// 壁
	std::vector<Math::Triangle> walls{};
	// 床
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

		float cos = triangle.GetNormal().Dot(Math::Vector3::Up);

		// 法線の角度が 45° 以上なら床、でなければ壁
		if (Math::Abs(cos) > 1.0f / 1.41421356f)
		{
			floors.push_back(triangle);
		}
		else
		{
			walls.push_back(triangle);
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
	// 座標補正
	PositionCorrect(distance, direction);

	distance = Math::INFTY;
	direction = Math::Vector3::Zero;

	// 床
	for (const auto& floor : floors)
	{
		Math::Vector3 newDirection{};

		float newDistance = Math::Geometry::Distance(floor, box, &newDirection);

		// 最短距離を更新
		if (newDistance < distance)
		{
			distance = newDistance;
			direction = newDirection;
		}
	}
	// 座標補正
	PositionCorrect(distance, direction);

	// 次の状態に遷移
	std::unique_ptr<Systems::State<PlayerInternals>> nextState{ m_currentState->MoveNextState() };
	if (nextState)
	{
		PlayerState* pNextState = static_cast<PlayerState*>(nextState.release());
		SetState(std::unique_ptr<PlayerState>{ pNextState });
	}
}

// モデルの更新
void Player::UpdateModel(float elapsedTime)
{
	m_internals.pModel->SetTransform(*m_internals.pTransform);
	m_internals.pModel->Update(elapsedTime, *this);
}

// 意図を伸ばす
void Player::StretchThread(const Math::Vector3& targetPosition, bool isTarget)
{
	SetState(std::make_unique<PlayerStateStretchThread>());
}

// 中心座標を取得
const Math::Vector3& Player::GetPosition() const
{
	return m_internals.pBoxCollider->GetWorldBox().position;
}

// 当たり判定を取得
const Math::Box& Player::GetBox() const
{
	return m_internals.pBoxCollider->GetWorldBox();
}

// 移動速度の比率を取得
float Player::GetMoveVelocityRatio() const
{
	return m_internals.moveVelocity.Length() / m_internals.moveMaxSpeed.GetMin();
}

// カメラの目標座標を取得
Math::Vector3 Player::GetCameraTarget() const
{
	return GetPosition() + Math::Vector3::Up * GetBox().size.y + (m_internals.moveVelocity + m_internals.fallSpeed * Math::Vector3::Down) * m_internals.CAMERA_VELOCITY_COEFFICIENT;
}

// 状態を変更
void Player::SetState(std::unique_ptr<PlayerState> state)
{
	if (m_currentState != nullptr)
	{
		m_currentState->Exit(&m_internals);
	}
	m_currentState = std::move(state);
	m_currentState->Enter(&m_internals);
}
