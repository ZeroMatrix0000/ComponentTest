/*
 * FileName:     PlayerStateFalling.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/13
 *
 * プレイヤーの落下状態
 */

#pragma once

#include "PlayerState.h"

 // 状態
class PlayerStateFalling : public PlayerState
{

public:


	/* メンバ関数 */

	// コンストラクタ
	PlayerStateFalling();

	// 開始処理
	void Enter(PlayerInternals* pInternals) override;
	// 更新処理
	void Update(PlayerInternals* pInternals, float elapsedTime) override;
	// 終了処理
	void Exit(PlayerInternals* pInternals) override;

	// 座標補正
	void PositionCorrect(PlayerInternals* pInternals, float distance, const Math::Vector3& direction) override;

};
