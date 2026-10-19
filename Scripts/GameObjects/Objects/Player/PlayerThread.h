/*
 * FileName:     PlayerThread.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/19
 *
 * プレイヤーの糸
 */

#pragma once

#include "Scripts/Commons/Components/Component.h"

namespace Components
{
	class Transform;
}
namespace Renderings
{
	class ParticleManager;
	class ICameraScreen;
}

// プレイヤーの糸
class PlayerThread : public Component
{

public:


	/* 列挙型 */

	// 状態
	enum class State
	{
		None,
		Expand,
		Stay,
		Contract,
		AirContract
	};


	/* メンバ関数 */

	// コンストラクタ
	PlayerThread(const ComponentDesc& desc);

	// 初期化処理
	void Initalize(const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder) override;

	// 更新処理
	void Update(float elapsedTime);

	// 移動
	void Translate(float elapsedTime);
	// 回転
	void Rotate(float elapsedTime);
	// スケーリング
	void Scaling(float elapsedTime);

	// 映るカメラ画面を追加
	void AddICameraScreen(const Renderings::ICameraScreen& iCameraScreen);

	// ターゲットを設定
	void SetTarget(const Math::Vector3& playerPosition, const Math::Vector3& targetPosition, const Math::Quaternion& rotation, bool toAir);

	// 非表示
	void UnDisplay() { m_isDisplay = false; }

	// プレイヤー位置を取得
	const Math::Vector3& GetPlayerPosition() const { return m_playerPosition; }

	// 移動速度を取得
	Math::Vector3 GetPlayerVelocity() const { return m_direction * m_contractSpeed; }

	// 現在の状態を取得
	State GetCurrentState() const { return m_currentState; }


private:


	/* メンバ変数 */

	// 糸の太さ
	float THICKNESS;
	// 糸が伸びる速さ
	float EXPAND_SPEED;
	// 糸が回転する速さ
	float ROTATION_SPEED;
	// 待ち時間
	float STAY_TIME;
	// 糸を縮める加速度
	float CONTRACT_ACCELERATION;
	// 糸を離す長さ
	float MIN_LENGTH;

	// 距離
	float m_distance;
	// 糸の長さ
	float m_length;

	// プレイヤーの位置
	Math::Vector3 m_playerPosition;
	// ターゲットの位置
	Math::Vector3 m_targetPosition;
	// 現在位置
	Math::Vector3 m_currentPosition;
	// 糸を伸ばす方向
	Math::Vector3 m_direction;
	
	// 回転
	Math::Quaternion m_baseRotation;
	// 糸の回転
	Cycled m_rotationAngle;

	// 待ち時間
	float m_stayTimer;

	// 縮む速さ
	float m_contractSpeed;

	// 状態
	State m_currentState;

	// 空中を指しているか
	bool m_toAir;

	// パーティクル出現タイマー
	float m_particleTimer;

	// 表示するかどうか
	bool m_isDisplay;

	// トランスフォーム
	Components::Transform* m_pTransform;

	// パーティクル管理
	Renderings::ParticleManager* m_pParticleManager;
	// パーティクル管理のトランスフォーム
	Transform* m_pParticleManagerTransform;

};
