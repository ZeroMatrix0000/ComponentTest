/*
 * FileName:     PlayerStateOnGround.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/12
 *
 * プレイヤーの地面にいる状態
 */

#pragma once

#include "PlayerState.h"

	// 状態
class PlayerStateOnGround : public PlayerState
{

public:


	/* メンバ関数 */

	// コンストラクタ
	PlayerStateOnGround();

	// 開始処理
	void Enter(PlayerInternals* pInternals) override;
	// 更新処理
	void Update(PlayerInternals* pInternals, float elapsedTime) override;
	// 終了処理
	void Exit(PlayerInternals* pInternals) override;


protected:


	/* メンバ関数 */

	// 座標補正
	void PositionCorrect(PlayerInternals* pInternals, float distance, const Math::Vector3& direction) override;

};
