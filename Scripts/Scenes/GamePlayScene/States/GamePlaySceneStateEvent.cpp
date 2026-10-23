/*
 * FileName:     GamePlaySceneStateEvent.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/23
 *
 * ゲームプレイシーンのイベント状態
 */

#include "Pch.h"
#include "GamePlaySceneStateEvent.h"

#include "GamePlaySceneStates.h"
#include "../GamePlaySceneInternals.h"
#include "../Actions/ActionManager.h"
#include "Scripts/GameObjects/Objects/Player/Player.h"
#include "Scripts/GameObjects/Objects/Player/PlayerCamera.h"
#include "Scripts/GameObjects/Objects/NPC/NPC.h"
#include "Scripts/GameObjects/Objects/NPC/NPCManager.h"
#include "Scripts/GameObjects/UIs/Z2Talk/Z2Talk.h"
#include "Scripts/GameObjects/UIs/BlackBelt/BlackBelt.h"
#include "Scripts/GameObjects/UIs/MessageWindow/MessageWindow.h"
#include "Scripts/GameObjects/UIs/TargetPoint/TargetPoint.h"
#include "Scripts/Main/GameContext.h"
#include "Scripts/Main/IGameInput.h"
#include "Scripts/Commons/Colliders/MeshCollider.h"

// コンストラクタ
GamePlaySceneStateEvent::GamePlaySceneStateEvent()
{
}

// 開始処理
void GamePlaySceneStateEvent::Enter(GamePlaySceneInternals* pInternals)
{
	// イベントを設定
	pInternals->actionManagerRef->LoadEvent(pInternals->nPCManagerRef->GetPFocusedNPC()->GetEventName());

	// レティクルを非表示
	pInternals->reticleRef->SetColor(DirectX::Colors::Transparent);

	// カーソルを隠す
	pInternals->nPCManagerRef->HideCursor(true);

	// 黒帯を出現させる
	pInternals->blackBeltRef->Appear(true);

	// ターゲット照準を非表示
	pInternals->targetPointRef->Appear(false);
}

// 更新処理
void GamePlaySceneStateEvent::Update(GamePlaySceneInternals* pInternals, float elapsedTime)
{
	// NPCの更新
	pInternals->nPCManagerRef->SetRotation(pInternals->playerRef->GetPosition());
	pInternals->nPCManagerRef->Update(elapsedTime);
	pInternals->nPCManagerRef->SetCursor(pInternals->iCameraScreenRef.Get());

	// ゲーム入力
	auto* pIGameInput = pInternals->gameContextRef->GetPIGameInput();

	// プレイヤーの更新
	RotatePlayer(pInternals);
	pInternals->playerRef->Update(elapsedTime, Math::Vector3::Zero, false, false);
	pInternals->nPCManagerRef->PlayerCollide(&pInternals->playerRef.Get());
	pInternals->playerRef->MeshCorrect(pInternals->groundMeshRef->GetWorldMesh());
	pInternals->playerRef->UpdateModel(elapsedTime);

	// はなすの更新
	pInternals->z2TalkRef->SetPosition(pInternals->playerRef->GetBox(), pInternals->iCameraScreenRef.Get());
	pInternals->z2TalkRef->Update(elapsedTime, false);

	// 上下の黒帯の更新
	pInternals->blackBeltRef->Update(elapsedTime);

	// カメラの更新
	pInternals->playerCameraRef->SetTarget
	(
		(pInternals->playerRef->GetCameraTarget() + pInternals->nPCManagerRef->GetPFocusedNPC()->GetPosition()) / 2.0f
	);
	pInternals->playerCameraRef->Rotate(pIGameInput->GetPlayerCameraRotate());
	pInternals->playerCameraRef->MeshCorrect(pInternals->groundMeshRef->GetWorldMesh(), pInternals->playerRef->GetPosition());
	pInternals->playerCameraRef->Update(elapsedTime);
	pInternals->iCameraScreenRef->UpdateViewMatrix();

	// メッセージウィンドウの更新
	pInternals->messageWindowRef->Update(elapsedTime);

	// アクション管理の更新
	pInternals->actionManagerRef->Update(elapsedTime, *pIGameInput);
	// 全てのアクションが終了したら状態遷移
	if (pInternals->actionManagerRef->IsCompleted())
	{
		SetNextState(std::make_unique<GamePlaySceneStateField>());
	}
}

// 終了処理
void GamePlaySceneStateEvent::Exit(GamePlaySceneInternals* pInternals)
{
	// カーソルを表示
	pInternals->nPCManagerRef->HideCursor(false);

	// 黒帯を隠す
	pInternals->blackBeltRef->Appear(false);
}

// プレイヤーを会話相手の方に向ける
void GamePlaySceneStateEvent::RotatePlayer(GamePlaySceneInternals* pInternals)
{
	// プレイヤーを会話相手の方に向ける
	// 向く方向
	Math::Vector3 direction = pInternals->nPCManagerRef->GetPFocusedNPC()->GetBox().position - pInternals->playerRef->GetPosition();
	// Y軸方向は無視
	direction.y = 0.0f;
	// 正規化
	direction.Normalize();
	// 回転
	Math::Quaternion rotation = Math::Quaternion::FromToRotation(Math::Vector3::Backward, direction);
	pInternals->playerRef->SetRotation(rotation);
}
