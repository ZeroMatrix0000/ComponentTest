/*
 * FileName:     PlayerStateExpandThread.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/16
 *
 * プレイヤーの糸を伸ばす状態
 */

#pragma once

#include "PlayerState.h"
#include "../PlayerInternals.h"

 // プレイヤーの糸を伸ばす状態
class PlayerStateExpandThread : public PlayerState
{

public:


	/* メンバ関数 */

	// コンストラクタ
	PlayerStateExpandThread(const Math::Vector3& targetPosition, PlayerInternals::TargetType targetType);

	// 開始処理
	void Enter(PlayerInternals* pInternals) override;
	// 更新処理
	void Update(PlayerInternals* pInternals, float elapsedTime) override;
	// 終了処理
	void Exit(PlayerInternals* pInternals) override;

	// 座標補正
	void PositionCorrect(PlayerInternals* pInternals, float distance, const Math::Vector3& direction) override;


private:


	/* メンバ変数 */

	// 目標値
	Math::Vector3 m_targetPosition;
	// ターゲットの種類
	PlayerInternals::TargetType m_targetType;

	// プレイヤーの座標
	Math::Vector3 m_playerPosition;

};
