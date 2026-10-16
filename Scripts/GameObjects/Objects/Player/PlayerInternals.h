/*
 * FileName:     PlayerInternals.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/16
 *
 * プレイヤーの内部データ
 */

#pragma once

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
class PlayerThread;

struct PlayerInternals
{

	/* 列挙型 */

	// 落下状態
	enum class FallState
	{
		OnGround,
		OnAir,
		Falling
	};


	/* 列挙型 */

	// ターゲットの種類
	enum class TargetType
	{
		Object,
		Ground,
		Air
	};


	/* メンバ変数 */

	// 移動加速度
	float MOVE_ACCELERATION{};
	// ダッシュ時の速度倍率
	float DASH_RATIO{};

	// 落下加速度
	float FALL_ACCELERATION{};

	// ジャンプ力
	float JUMP_POWER{};

	// カメラの速度の参照量
	float CAMERA_VELOCITY_COEFFICIENT{};

	// 糸を伸ばしたときのインターバル
	float THREAD_INTERVAL{};

	// カメラ画面のポインタ
	const Renderings::CameraScreen<Camera::EulerTargetCamera>* P_CAMERA_SCREEN{};

	// 移動入力
	Math::Vector3 MOVE{};
	// ダッシュ入力
	bool IS_DASH{};
	// ジャンプ入力入力
	bool IS_JUMP{};

	// 移動速度
	Math::Vector3 moveVelocity{};
	// 移動の速さの最大
	Limited moveMaxSpeed{};
	// 落下速度
	Limited fallSpeed{};

	// プレイヤーの回転
	Spring<Math::Quaternion> rotation{};

	// 落下猶予
	Limited fallCoyoteTime{};
	// ジャンプの先行入力猶予
	Limited jumpBufferTime{};
	// 糸伸ばしタイマー
	Limited threadTimer{};

	// トランスフォーム
	Transform* pTransform{};
	// 長方形の当たり判定
	Colliders::BoxCollider* pBoxCollider{};
	// パーティクル管理
	Renderings::ParticleManager* pParticleManager{};

	// プレイヤーのモデル
	PlayerModel* pModel{};

	// プレイヤーの糸
	PlayerThread* pThread{};

};
