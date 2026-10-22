/*
 * FileName:     Player.h
 * Author:       Takao Hayata
 * Last Updated: 2026/09/18
 *
 * プレイヤー
 */

#pragma once

#include "PlayerInternals.h"
#include "States/PlayerState.h"
#include "Scripts/Commons/Components/Component.h"

 // プレイヤー
class Player : public Component
{

public:


	/* メンバ関数 */

	// コンストラクタ
	Player(const ComponentDesc& desc);

	// 初期化処理
	void Initalize(const nlohmann::ordered_json& json, Ref<IGameObjectFinder> iGameObjectFinder) override;

	// 更新処理
	void Update(float elapsedTime, const Math::Vector3& move, bool isDash, bool isJump);

	// 座標補正
	void PositionCorrect(float distance, const Math::Vector3& direction);
	// メッシュによる座標補正
	void MeshCorrect(const Mesh& mesh);

	// モデルの更新
	void UpdateModel(float elapsedTime = 0.0f);

	// 糸を伸ばす
	void ExpandWire(const Math::Vector3& targetPosition, PlayerInternals::TargetType targetType);

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

	// 移動速度を取得
	Math::Vector3 GetMoveVelocity() const;

	// 状態が一致しているか
	template<typename T> requires IsDerived<T, PlayerState>
	bool IsState() const { return dynamic_cast<T*>(m_currentState.get()); }

	// カメラの目標座標を取得
	Math::Vector3 GetCameraTarget() const;

	// 糸を伸ばすことができるかどうか
	bool CanExpandWire() const { return m_internals.canExpandWire; }


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
