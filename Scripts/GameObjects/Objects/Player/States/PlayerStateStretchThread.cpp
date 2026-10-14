/*
 * FileName:     PlayerStateStretchThread.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/14
 *
 * プレイヤーの地面にいる状態
 */

#include "Pch.h"
#include "PlayerStates.h"

#include "../PlayerInternals.h"
#include "Scripts/Commons/Components/Transform.h"
#include "Scripts/Commons/Colliders/BoxCollider.h"
#include "Scripts/Commons/Renderings/Particle/ParticleManager.h"
#include "Scripts/GameObjects/Particles/JumpingParticle.h"

 // コンストラクタ
PlayerStateStretchThread::PlayerStateStretchThread()
{
}

// 開始処理
void PlayerStateStretchThread::Enter(PlayerInternals* pInternals)
{
}

// 更新処理
void PlayerStateStretchThread::Update(PlayerInternals* pInternals, float elapsedTime)
{
}

// 終了処理
void PlayerStateStretchThread::Exit(PlayerInternals* pInternals)
{
}

// 座標補正
void PlayerStateStretchThread::PositionCorrect(PlayerInternals* pInternals, float distance, const Math::Vector3& direction)
{
}
