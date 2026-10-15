/*
 * FileName:     PlayerThread.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/15
 *
 * プレイヤーの糸
 */

#pragma once

#include "Scripts/Commons/Components/Component.h"

namespace Components
{
	class Transform;
}

// プレイヤーの糸
class PlayerThread : public Component
{

public:


	/* メンバ関数 */

	// コンストラクタ
	PlayerThread(const ComponentDesc& desc);

	// 初期化処理
	void Initalize(const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder) override;

	// 更新処理
	void Update(float elapsedTime);

	// ターゲットを設定
	void SetTarget(const Math::Vector3& playerPosition, const Math::Vector3& targetPosition, const Math::Quaternion& rotation);


private:


	/* メンバ変数 */

	// 糸の太さ
	float m_thickness;
	// 糸が伸びる速さ
	float m_stretchSpeed;
	// 糸が回転する速さ
	float m_rotationSpeed;

	// 距離
	float m_distance;

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

	// トランスフォーム
	Components::Transform* m_pTransform;

};
