/*
 * FileName:     Player.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/12
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
	float jumpPower = -m_internals.fallSpeed.GetMin();
	float coyoteTime = m_internals.fallCoyoteTime.GetMax();

	Systems::JsonSerializer serializer{ pIGameObjectFinder };
	serializer.AddParameter(&moveMaxSpeed, "MoveMaxSpeed");
	serializer.AddParameter(&m_internals.MOVE_ACCELERATION, "MoveAcceleration");
	serializer.AddParameter(&m_internals.DASH_RATIO, "DashRatio");
	serializer.AddParameter(&fallMaxSpeed, "FallMaxSpeed");
	serializer.AddParameter(&m_internals.FALL_ACCELERATION, "FallAcceleration");
	serializer.AddParameter(&jumpPower, "JumpPower");
	serializer.AddParameter(&m_internals.CAMERA_VELOCITY_COEFFICIENT, "CameraVelocityCoefficient");
	serializer.AddParameter(&coyoteTime, "CoyoteTime");
	serializer.AddParameter(&m_internals.P_CAMERA_SCREEN, "CameraScreen");
	serializer.Load(json);

	m_internals.moveMaxSpeed.Initialize(0.0f, moveMaxSpeed, moveMaxSpeed * m_internals.DASH_RATIO);

	m_internals.fallSpeed.Initialize(0.0f, -jumpPower, fallMaxSpeed);

	m_internals.fallState = PlayerInternals::FallState::OnGround;
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
		m_internals.pModel->GetPOwner()->GetComponent<Renderings::Model3D>()->AddICameraScreen(*m_internals.P_CAMERA_SCREEN);
	}

	// 現在の状態を初期化
	//SetState(std::make_unique<GamePlaySceneStateField>());
}

// 更新処理
void Player::Update(float elapsedTime, const Math::Vector3& move, bool isDash, bool isJump)
{
	// 入力を受け取る
	m_internals.move = move;
	m_internals.isDash = isDash;
	m_internals.isJump = isJump;

	// カメラのY軸回転
	Math::Quaternion cameraRotation = Math::Quaternion::CreateFromAxisAngle(Math::Vector3::UnitY, Math::Deg2Rad(m_internals.P_CAMERA_SCREEN->GetCamera().rotation.y));

	// 最大の早さを調整
	m_internals.moveMaxSpeed += m_internals.MOVE_ACCELERATION* elapsedTime * m_internals.DASH_RATIO * (isDash ? 1.0f : -1.0f);

	// 加速度がない
	if (move == Math::Vector3::Zero)
	{
		// 速さ
		Limited speed = Limited::Create(m_internals.moveVelocity.Length() - m_internals.MOVE_ACCELERATION* elapsedTime, 0.0f, m_internals.moveMaxSpeed);
		m_internals.moveVelocity.Normalize();
		m_internals.moveVelocity *= speed;
	}
	// 加速度がある
	else
	{
		// 速度に加速度を足す
		m_internals.moveVelocity += Math::Vector3::Transform(move, cameraRotation) * m_internals.MOVE_ACCELERATION* elapsedTime * (isDash ? m_internals.DASH_RATIO : 1.0f);
		// 速さを最大に収める
		if (m_internals.moveVelocity.Length() > m_internals.moveMaxSpeed)
		{
			m_internals.moveVelocity.Normalize();
			m_internals.moveVelocity *= m_internals.moveMaxSpeed;
		}

		m_internals.rotation.SetTarget
		(
			Math::Quaternion::CreateFromAxisAngle(Math::Vector3::UnitY, Math::Arctan2(m_internals.moveVelocity.x, m_internals.moveVelocity.z)),
			200.0f,
			25.0f
		);
	}

	m_internals.fallSpeed += m_internals.FALL_ACCELERATION * elapsedTime;

	// ジャンプ入力
	if (isJump)
	{
		m_internals.jumpBufferTime = m_internals.jumpBufferTime.GetMax();
	}

	m_internals.jumpBufferTime -= elapsedTime;

	// ジャンプ
	if (m_internals.jumpBufferTime != 0.0f && m_internals.fallState != PlayerInternals::FallState::Falling)
	{
		m_internals.fallState = PlayerInternals::FallState::Falling;
		m_internals.fallSpeed = m_internals.fallSpeed.GetMin();
		// 砂煙の再生
		m_internals.pParticleManager->Play<JumpingParticle>();
	}

	// 回転の更新
	m_internals.rotation.Tick(elapsedTime);

	// 位置に速度を足す
	m_internals.pTransform->Translate((m_internals.moveVelocity - Math::Vector3::UnitY * m_internals.fallSpeed) * elapsedTime);
	// 回転させる
	m_internals.pTransform->SetRotation(m_internals.rotation.GetCurrent());

	// 落下猶予
	m_internals.fallCoyoteTime -= elapsedTime;

	// 当たり判定の更新
	m_internals.pBoxCollider->ApplyTransform();

	// パーティクルの更新
	m_internals.pParticleManager->Update(elapsedTime);

	//// 次の状態に遷移
	//std::unique_ptr<Systems::State<GamePlaySceneInternals>> nextState{ m_currentState->MoveNextState() };
	//if (nextState)
	//{
	//	SetState(std::move(nextState));
	//}

	//// 状態ごとの処理
	//m_currentState->Update(m_internals.get(), elapsedTime);
}

// 直方体による座標補正
void Player::BoxCorrect(const std::vector<const Math::Box*>& pBoxes)
{
	// 直方体
	Math::Box box = m_internals.pBoxCollider->GetWorldBox();

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
			m_internals.pTransform->Translate(direction * distance);
			box.position += direction * distance;

			// 押出方向が上かつ落下中のとき
			if (direction.Dot(Math::Vector3::Down) > 1.0f / 1.41421356f && m_internals.fallSpeed > 0.0f)
			{
				// 落下中なら砂煙を再生
				if (m_internals.fallState == PlayerInternals::FallState::Falling)
				{
					m_internals.pParticleManager->Play<LandingParticle>();
				}

				m_internals.fallSpeed = 0.0f;
				m_internals.fallState = PlayerInternals::FallState::OnGround;
				m_internals.fallCoyoteTime = m_internals.fallCoyoteTime.GetMax();
			}
		}
	}

	// 当たり判定の更新
	m_internals.pBoxCollider->ApplyTransform();
}

// メッシュによる座標補正
void Player::MeshCorrect(const Mesh& mesh)
{
	// 直方体
	Math::Box box = m_internals.pBoxCollider->GetWorldBox();
	// 外接する非回転直方体（少し広めに）
	Math::NonRotatingBox nonRotatingBox = m_internals.pBoxCollider->GetWorldBox().CreateNonRotatingBox();
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
		m_internals.pTransform->Translate(direction * distance);
		box.position += direction * distance;

		// 押出方向が上かつ落下中のとき
		if (direction.Dot(Math::Vector3::Down) > 1.0f / 1.41421356f && m_internals.fallSpeed > 0.0f)
		{
			// 落下中なら砂煙を再生
			if (m_internals.fallState == PlayerInternals::FallState::Falling)
			{
				m_internals.pParticleManager->Play<LandingParticle>();
			}

			m_internals.fallSpeed = 0.0f;
			m_internals.fallState = PlayerInternals::FallState::OnGround;
			m_internals.fallCoyoteTime = m_internals.fallCoyoteTime.GetMax();
		}
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
	switch (m_internals.fallState)
	{
	case PlayerInternals::FallState::OnGround:
		// 地面に貼り付ける
		if (distance < box.size.y / 8.0f)
		{
			float arccos = direction.Dot(Math::Vector3::UnitY);

			if (distance < 0.0f || Math::Sign(arccos) == -1)
			{
				m_internals.pTransform->Translate(Math::Vector3::UnitY * (Math::Sign(arccos) * distance));
				box.position += Math::Vector3::UnitY * (Math::Sign(arccos) * distance);
				m_internals.fallSpeed = 0.0f;
				m_internals.fallCoyoteTime = m_internals.fallCoyoteTime.GetMax();
				break;
			}
		}
		m_internals.fallState = PlayerInternals::FallState::OnAir;
		break;
	case PlayerInternals::FallState::OnAir:
		if (distance < 0.0f)
		{
			float arccos = direction.Dot(Math::Vector3::UnitY);

			m_internals.pTransform->Translate(Math::Vector3::UnitY * (Math::Sign(arccos) * distance));
			box.position += Math::Vector3::UnitY * (Math::Sign(arccos) * distance);
			m_internals.fallSpeed = 0.0f;
			m_internals.fallState = PlayerInternals::FallState::OnGround;
			m_internals.fallCoyoteTime = m_internals.fallCoyoteTime.GetMax();
			break;
		}
		if (m_internals.fallCoyoteTime.IsMin())
		{
			m_internals.fallState = PlayerInternals::FallState::Falling;
		}
		break;
	case PlayerInternals::PlayerInternals::FallState::Falling:
		if (distance < 0.0f)
		{
			float arccos = direction.Dot(Math::Vector3::UnitY);

			m_internals.pTransform->Translate(Math::Vector3::UnitY * (Math::Sign(arccos) * distance));
			box.position += Math::Vector3::UnitY * (Math::Sign(arccos) * distance);
			// 落下中のとき
			if (m_internals.fallSpeed > 0.0f)
			{
				m_internals.fallSpeed = 0.0f;
				m_internals.fallState = PlayerInternals::FallState::OnGround;
				m_internals.fallCoyoteTime = m_internals.fallCoyoteTime.GetMax();
				// 砂煙の再生
				m_internals.pParticleManager->Play<LandingParticle>();
			}
			break;
		}
		break;
	default:
		break;
	}

	// 当たり判定の更新
	m_internals.pBoxCollider->ApplyTransform();
}

// モデルの更新
void Player::UpdateModel(float elapsedTime)
{
	m_internals.pModel->SetTransform(*m_internals.pTransform);
	m_internals.pModel->Update(elapsedTime, *this);
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
	return GetPosition() + (m_internals.moveVelocity + m_internals.fallSpeed * Math::Vector3::Down) * m_internals.CAMERA_VELOCITY_COEFFICIENT;
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
