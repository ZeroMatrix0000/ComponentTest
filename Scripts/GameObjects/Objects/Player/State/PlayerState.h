/*
 * FileName:     PlayerState.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/12
 *
 * プレイヤーの状態
 */

#pragma once

#include "Scripts/Commons/Systems/State.h"

struct PlayerInternals;

	// 状態
class PlayerState : public Systems::State<PlayerInternals>
{

public:


	/* メンバ関数 */

	// コンストラクタ
	PlayerState() = default;
	// デストラクタ
	virtual ~PlayerState() = default;


protected:


	/* メンバ関数 */

	// 座標補正
	virtual void PositionCorrect(PlayerInternals* pInternals, float distance, const Math::Vector3& direction) = 0;

};
