/*
 * FileName:     GamePlaySceneStateField.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/23
 *
 * ゲームプレイシーンのフィールド状態
 */

#include "Pch.h"
#include "GamePlaySceneStateField.h"

#include "../GamePlaySceneInternals.h"
#include "GamePlaySceneStates.h"
#include "Scripts/Main/IsDerivedRegister.h"
#include "Scripts/GameObjects/Objects/Player/Player.h"
#include "Scripts/GameObjects/Objects/Player/PlayerCamera.h"
#include "Scripts/GameObjects/Objects/Player/States/PlayerStates.h"
#include "Scripts/GameObjects/Objects/NPC/NPCManager.h"
#include "Scripts/GameObjects/UIs/Z2Talk/Z2Talk.h"
#include "Scripts/GameObjects/UIs/BlackBelt/BlackBelt.h"
#include "Scripts/GameObjects/UIs/MessageWindow/MessageWindow.h"
#include "Scripts/GameObjects/UIs/TargetPoint/TargetPoint.h"
#include "Scripts/Main/GameContext.h"
#include "Scripts/Main/IGameInput.h"
#include "Scripts/Commons/Colliders/MeshCollider.h"

// コンストラクタ
GamePlaySceneStateField::GamePlaySceneStateField()
{
}

// 開始処理
void GamePlaySceneStateField::Enter(GamePlaySceneInternals* pInternals)
{
	// レティクルを表示
	pInternals->reticleRef->SetColor(DirectX::Colors::White);
	// ターゲット照準を表示
	pInternals->targetPointRef->Appear(true);
}

// 更新処理
void GamePlaySceneStateField::Update(GamePlaySceneInternals* pInternals, float elapsedTime)
{
	// ゲーム入力
	auto* pIGameInput = pInternals->gameContextRef->GetPIGameInput();

	// 糸伸ばし入力で糸を伸ばす
	if (pIGameInput->GetPlayerExpandWire() && pInternals->playerRef->CanExpandWire())
	{
		TargetPoint::Info info = pInternals->targetPointRef->GetTargetInfo();
		pInternals->playerRef->ExpandWire(info.position, static_cast<PlayerInternals::TargetType>(info.type));
	}

	// NPCの更新
	pInternals->nPCManagerRef->SetRotation(pInternals->playerRef->GetPosition());
	pInternals->nPCManagerRef->Update(elapsedTime);
	pInternals->nPCManagerRef->SetCursor(pInternals->iCameraScreenRef.Get());

	// プレイヤーの更新
	pInternals->playerRef->Update(elapsedTime, pIGameInput->GetPlayerMove(), pIGameInput->GetPlayerDash(), pIGameInput->GetPlayerJump());
	pInternals->nPCManagerRef->PlayerCollide(&pInternals->playerRef.Get());
	pInternals->playerRef->MeshCorrect(pInternals->groundMeshRef->GetWorldMesh());
	pInternals->playerRef->UpdateModel(elapsedTime);

	// カメラの更新
	pInternals->playerCameraRef->SetTarget(pInternals->playerRef->GetCameraTarget());
	pInternals->playerCameraRef->Rotate(pIGameInput->GetPlayerCameraRotate());
	pInternals->playerCameraRef->MeshCorrect(pInternals->groundMeshRef->GetWorldMesh(), pInternals->playerRef->GetPosition());
	pInternals->playerCameraRef->Update(elapsedTime);
	pInternals->iCameraScreenRef->UpdateViewMatrix();

	// プレイヤーが話せる状態か
	bool canSpeak = pInternals->playerRef->IsState<PlayerStateOnGround>();

	// 糸伸ばし中ならカーソルを非表示
	pInternals->nPCManagerRef->HideCursor(!canSpeak);

	// はなすの更新
	pInternals->z2TalkRef->SetPosition(pInternals->playerRef->GetBox(), pInternals->iCameraScreenRef.Get());
	pInternals->z2TalkRef->Update(elapsedTime, pInternals->nPCManagerRef->GetPFocusedNPC() != nullptr && canSpeak);

	// 上下の黒帯の更新
	pInternals->blackBeltRef->Update(elapsedTime);

	// 話しかけられるNPC
	const NPC* pFocusedNPC = pInternals->nPCManagerRef->GetPFocusedNPC();
	// 糸を伸ばしていないかつ
	// NPCに話しかけられる状態かつ
	// プレイヤーが話せる状態かつ
	// インタラクト入力がされたならイベント状態へ
	if
		(
			pFocusedNPC &&
			canSpeak &&
			pIGameInput->GetFieldInteract()
			)
	{
		SetNextState(std::make_unique<GamePlaySceneStateEvent>());
	}

	// メッセージウィンドウの更新
	pInternals->messageWindowRef->Update(elapsedTime);

	// ターゲットの更新
	pInternals->targetPointRef->Update(elapsedTime);
}

// 終了処理
void GamePlaySceneStateField::Exit(GamePlaySceneInternals* pInternals)
{
}
