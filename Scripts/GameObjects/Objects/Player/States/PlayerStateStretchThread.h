/*
 * FileName:     PlayerStateStretchThread.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/15
 *
 * プレイヤーの糸を伸ばす状態
 */

#pragma once

#include "PlayerState.h"
#include "../PlayerInternals.h"

 // 状態
class PlayerStateStretchThread : public PlayerState
{

public:


	/* メンバ関数 */

	// コンストラクタ
	PlayerStateStretchThread(const Math::Vector3& targetPosition, PlayerInternals::TargetType targetType);

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

};
