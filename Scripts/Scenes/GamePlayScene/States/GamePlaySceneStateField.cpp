/*
 * FileName:     GamePlaySceneStateField.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/16
 *
 * ゲームプレイシーンのフィールド状態
 */

#include "Pch.h"
#include "GamePlaySceneStateField.h"

#include "../GamePlaySceneInternals.h"
#include "GamePlaySceneStates.h"
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
	pInternals->pReticle->SetColor(DirectX::Colors::White);
	// ターゲット照準を表示
	pInternals->pTargetPoint->Appear(true);
}

// 更新処理
void GamePlaySceneStateField::Update(GamePlaySceneInternals* pInternals, float elapsedTime)
{
	// ゲーム入力
	auto* pIGameInput = pInternals->gameContext.GetPIGameInput();

	// 糸伸ばし入力で糸を伸ばす
	if (pIGameInput->GetPlayerExpandWire() && pInternals->pPlayer->CanExpandWire())
	{
		TargetPoint::Info info = pInternals->pTargetPoint->GetTargetInfo();
		pInternals->pPlayer->ExpandWire(info.position, static_cast<PlayerInternals::TargetType>(info.type));
	}

	// NPCの更新
	pInternals->pNPCManager->SetRotation(pInternals->pPlayer->GetPosition());
	pInternals->pNPCManager->Update(elapsedTime);
	pInternals->pNPCManager->SetCursor(*pInternals->pCameraScreen);

	// プレイヤーの更新
	pInternals->pPlayer->Update(elapsedTime, pIGameInput->GetPlayerMove(), pIGameInput->GetPlayerDash(), pIGameInput->GetPlayerJump());
	pInternals->pNPCManager->PlayerCollide(pInternals->pPlayer);
	pInternals->pPlayer->MeshCorrect(pInternals->pGroundMesh->GetWorldMesh());
	pInternals->pPlayer->UpdateModel(elapsedTime);

	// カメラの更新
	pInternals->pPlayerCamera->SetTarget(pInternals->pPlayer->GetCameraTarget());
	pInternals->pPlayerCamera->Rotate(pIGameInput->GetPlayerCameraRotate());
	pInternals->pPlayerCamera->MeshCorrect(pInternals->pGroundMesh->GetWorldMesh(), pInternals->pPlayer->GetPosition());
	pInternals->pPlayerCamera->Update(elapsedTime);
	pInternals->pCameraScreen->UpdateViewMatrix();

	// プレイヤーが話せる状態か
	bool canSpeak = pInternals->pPlayer->IsState<PlayerStateOnGround>();

	// 糸伸ばし中ならカーソルを非表示
	pInternals->pNPCManager->HideCursor(!canSpeak);

	// はなすの更新
	pInternals->pZ2Talk->SetPosition(pInternals->pPlayer->GetBox(), *pInternals->pCameraScreen);
	pInternals->pZ2Talk->Update(elapsedTime, pInternals->pNPCManager->GetPFocusedNPC() != nullptr && canSpeak);

	// 上下の黒帯の更新
	pInternals->pBlackBelt->Update(elapsedTime);

	// 話しかけられるNPC
	const NPC* pFocusedNPC = pInternals->pNPCManager->GetPFocusedNPC();
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
	pInternals->pMessageWindow->Update(elapsedTime);

	// ターゲットの更新
	pInternals->pTargetPoint->Update(elapsedTime);
}

// 終了処理
void GamePlaySceneStateField::Exit(GamePlaySceneInternals* pInternals)
{
}
