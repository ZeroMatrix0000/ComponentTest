/*
 * FileName:     Player.h
 * Author:       Takao Hayata
 * Last Updated: 2026/09/11
 *
 * プレイヤー
 */

#pragma once

#include "PlayerParams.h"
#include "Scripts/Commons/Components/Component.h"
#include "Scripts/Commons/Renderings/CameraScreen.h"

namespace Components
{
	class Transform;
}
namespace Colliders
{
	class BoxCollider;
}
namespace Renderings
{
	class ParticleManager;
}

class PlayerModel;

 // プレイヤー
class Player : public Component
{

public:


	/* 列挙型 */

	// 落下状態
	enum class FallState
	{
		OnGround,
		OnAir,
		Falling
	};


	/* メンバ関数 */

	// コンストラクタ
	Player(const ComponentDesc& desc);

	// 初期化処理
	void Initalize(const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder) override;

	// 更新処理
	void Update(float elapsedTime, const Math::Vector3& move, bool isDash, bool isJump);

	// 直方体による座標補正
	void BoxCorrect(const std::vector<const Math::Box*>& pBoxes);
	// メッシュによる座標補正
	void MeshCorrect(const Mesh& mesh);

	// モデルの更新
	void UpdateModel(float elapsedTime = 0.0f);

	// 回転を設定
	void SetRotation(const Math::Quaternion& rotation) { m_rotation.SetTarget(rotation); }

	// 中心座標を取得
	const Math::Vector3& GetPosition() const;
	// 当たり判定を取得
	const Math::Box& GetBox() const;

	// 移動速度の比率を取得
	float GetMoveVelocityRatio() const;

	// 落下速度
	float GetFallSpeed() const { return m_fallSpeed; }

	// 落下状態を取得
	FallState GetFallState() const { return m_fallState; }

	// カメラの目標座標を取得
	Math::Vector3 GetCameraTarget() const;


private:


	/* メンバ変数 */

	// パラメータ
	PlayerParams m_params;

	// 移動速度
	Math::Vector3 m_moveVelocity;
	// 移動の速さの最大
	Limited m_moveMaxSpeed;
	// 落下速度
	Limited m_fallSpeed;

	// プレイヤーの回転
	Spring<Math::Quaternion> m_rotation;

	// 落下状態
	FallState m_fallState;
	// 落下猶予
	Limited m_fallCoyoteTime;
	// ジャンプの先行入力猶予
	Limited m_jumpBufferTime;

	// トランスフォーム
	Transform* m_pTransform;
	// 長方形の当たり判定
	Colliders::BoxCollider* m_pBoxCollider;
	// パーティクル管理
	Renderings::ParticleManager* m_pParticleManager;
	// カメラ画面のポインタ
	const Renderings::CameraScreen<Camera::EulerTargetCamera>* m_pCameraScreen;

	// プレイヤーのモデル
	PlayerModel* m_pModel;

};
