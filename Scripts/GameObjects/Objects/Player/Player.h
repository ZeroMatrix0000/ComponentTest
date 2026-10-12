/*
 * FileName:     Player.h
 * Author:       Takao Hayata
 * Last Updated: 2026/09/12
 *
 * プレイヤー
 */

#pragma once

#include "PlayerInternals.h"
#include "State/PlayerState.h"
#include "Scripts/Commons/Components/Component.h"

 // プレイヤー
class Player : public Component
{

public:


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
	void SetRotation(const Math::Quaternion& rotation) { m_internals.rotation.SetTarget(rotation); }

	// 中心座標を取得
	const Math::Vector3& GetPosition() const;
	// 当たり判定を取得
	const Math::Box& GetBox() const;

	// 移動速度の比率を取得
	float GetMoveVelocityRatio() const;

	// 落下速度
	float GetFallSpeed() const { return m_internals.fallSpeed; }

	// 落下状態を取得
	PlayerInternals::FallState GetFallState() const { return m_internals.fallState; }

	// カメラの目標座標を取得
	Math::Vector3 GetCameraTarget() const;


private:


	/* メンバ関数 */

	// 状態を変更
	void SetState(std::unique_ptr<PlayerState> state);


	/* メンバ変数 */

	// 内部データ
	PlayerInternals m_internals;

	// 状態
	std::unique_ptr<PlayerState> m_currentState;

};
