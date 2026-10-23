/*
 * FileName:     GamePlaySceneInitializer.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/23
 *
 * ゲームプレイシーンの初期化関数
 */

#include "Pch.h"
#include "GamePlaySceneInitializer.h"

#include "GamePlaySceneInternals.h"
#include "Scripts/Main/IsDerivedRegister.h"
#include "Scripts/GameObjects/Objects/Player/Player.h"
#include "Scripts/GameObjects/Objects/Player/PlayerCamera.h"
#include "Scripts/GameObjects/Objects/NPC/NPCManager.h"
#include "Scripts/GameObjects/UIs/Z2Talk/Z2Talk.h"
#include "Scripts/GameObjects/UIs/BlackBelt/BlackBelt.h"
#include "Scripts/GameObjects/UIs/MessageWindow/MessageWindow.h"
#include "Scripts/GameObjects/UIs/TargetPoint/TargetPoint.h"
#include "Scripts/Scenes/GamePlayScene/Actions/ActionManager.h"
#include "Scripts/Main/GameContext.h"
#include "Scripts/Commons/GameObjects/IGameObjectManager.h"
#include "Scripts/Commons/Colliders/MeshCollider.h"
#include "Scripts/Commons/Renderings/Canvas.h"
#include "Scripts/Commons/Renderings/Model3D/Model3D.h"
#include "Scripts/Commons/Renderings/Model3D/Model3DSource.h"
#include "Scripts/Commons/Renderings/Shader/PixelShader.h"
#include "Scripts/Commons/Renderings/Shader/ConstantBuffer.h"
#include "Scripts/Commons/Systems/IInput.h"
#include "Scripts/Commons/Systems/IWindowController.h"

void GamePlaySceneInitializer::operator()(GamePlaySceneInternals* pInternals) const
{
	// マウスをループ
	pInternals->gameContextRef->GetPIInput()->SetMousePositionLoop(true);
	pInternals->gameContextRef->GetPIInput()->DisplayCursor(false);

	// 出力サイズ
	const Math::Vector2& outputSize = pInternals->gameContextRef->GetPIWindowController()->GetOutputSize();

	// ゲームオブジェクト管理
	auto* pIGameObjectManager = pInternals->gameContextRef->GetPIGameObjectManager();

	pIGameObjectManager->SetGameObjectsRef(pInternals->gameObjectsRef);
	pIGameObjectManager->Load("Scene_GamePlay");

	// プレイヤーを取得
	pInternals->playerRef = pIGameObjectManager->Find("Player")->GetComponent<Player>();
	pInternals->playerRef->UpdateModel();

	// プレイヤーカメラを取得
	Ref<GameObject> camera = pIGameObjectManager->Find("PlayerCamera");
	pInternals->iCameraScreenRef = camera->GetComponent<Renderings::ICameraScreen>();
	pInternals->playerCameraRef = camera->GetComponent<PlayerCamera>();
	pInternals->playerCameraRef->SetPosition(pInternals->playerRef->GetCameraTarget());
	pInternals->iCameraScreenRef->UpdateViewMatrix();
	pInternals->iCameraScreenRef->SetProjectionMatrix(outputSize);

	// キャンバスを取得
	pInternals->canvasRef = pIGameObjectManager->Find("Canvas")->GetComponent<Renderings::Canvas>();
	pInternals->canvasRef->SetSize(outputSize);

	// NPCマネージャーを取得
	pInternals->nPCManagerRef = pIGameObjectManager->Find("NPCManager")->GetComponent<NPCManager>();

	// 地面を取得
	Ref<GameObject> ground = pIGameObjectManager->Find("Ground");
	pInternals->groundMeshRef = ground->GetComponent<Colliders::MeshCollider>();
	pInternals->groundModelRef = ground->GetComponent<Renderings::Model3D>();
	pInternals->groundMeshRef->ApplyTransform();

	// レティクルを取得
	pInternals->reticleRef = pIGameObjectManager->Find("Reticle")->GetComponent<Renderings::Image>();

	// はなすを取得
	pInternals->z2TalkRef = pIGameObjectManager->Find("Z2Talk")->GetComponent<Z2Talk>();
	// 上下の黒帯取得
	pInternals->blackBeltRef = pIGameObjectManager->Find("BlackBelt")->GetComponent<BlackBelt>();
	pInternals->blackBeltRef->SetWidth(pInternals->canvasRef->GetSize().x);

	// メッセージウィンドウを取得
	pInternals->messageWindowRef = pIGameObjectManager->Find("MessageWindow")->GetComponent<MessageWindow>();

	// ターゲット照準を取得
	pInternals->targetPointRef = pIGameObjectManager->Find("TargetPoint")->GetComponent<TargetPoint>();

	// アクション管理を取得
	pInternals->actionManagerRef = pIGameObjectManager->Find("ActionManager")->GetComponent<ActionManager>();

	// シェーダにプレイヤーの座標を設定
	Renderings::ConstantBuffer* pConstantBuffer = pInternals->groundModelRef->GetPModelSource()->GetPPixelShader()->GetConstantBuffer();
	pConstantBuffer->SetVariable("ShadowPosition", pInternals->playerRef->GetPosition() + Math::Vector3::Down * pInternals->playerRef->GetBox().size.y / 2.0f);
	pConstantBuffer->SetVariable("ShadowRadius", 0.75f);
	// NPC の当たり判定
	const auto nPCBoxes = pInternals->nPCManagerRef->GetPBoxes();
	for (size_t i = 0; i < nPCBoxes.size(); i++)
	{
		pConstantBuffer->SetVariable("ShadowPosition", nPCBoxes.at(i)->position + Math::Vector3::Down * nPCBoxes.at(i)->size.y / 2.0f, 16 * (i + 1));
		pConstantBuffer->SetVariable("ShadowRadius", 0.75f, 16 * (i + 1));
	}
	pConstantBuffer->SetVariable("ShadowCount", static_cast<int>(1 + nPCBoxes.size()));
}
