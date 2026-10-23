/*
 * FileName:     GamePlayScene.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/23
 *
 * ゲームプレイシーン
 */

#include "Pch.h"
#include "GamePlayScene.h"

#include "GamePlaySceneInternals.h"
#include "States/GamePlaySceneStateField.h"
#include "Scripts/Main/GameContext.h"
#include "Scripts/Main/IsDerivedRegister.h"
#include "Scripts/GameObjects/Objects/Player/Player.h"
#include "Scripts/GameObjects/Objects/Player/PlayerCamera.h"
#include "Scripts/GameObjects/Objects/NPC/NPCManager.h"
#include "Scripts/GameObjects/UIs/Z2Talk/Z2Talk.h"
#include "Scripts/GameObjects/UIs/BlackBelt/BlackBelt.h"
#include "Scripts/Commons/Scenes/ISceneManager.h"
#include "Scripts/Commons/Renderings/Canvas.h"
#include "Scripts/Commons/Renderings/Model3D/Model3D.h"
#include "Scripts/Commons/Renderings/Model3D/Model3DSource.h"
#include "Scripts/Commons/Renderings/Shader/PixelShader.h"
#include "Scripts/Commons/Renderings/Shader/ConstantBuffer.h"
#include "Scripts/Commons/Systems/IWindowController.h"
#include "Scripts/Commons/Systems/IInput.h"

 // コンストラクタ
GamePlayScene::GamePlayScene(const ComponentDesc& desc)
	: Scene{ desc }
	, m_internals{ GetGameObjects(), GetContext() }
	, m_currentState{}
	, m_Initializer{}
{
}

// 初期化処理
void GamePlayScene::Initialize(const SceneTransitionData& data)
{
	// 初期化
	m_Initializer(&m_internals);

	// 現在の状態を初期化
	SetState(std::make_unique<GamePlaySceneStateField>());
}

// 更新処理
void GamePlayScene::Update(float elapsedTime)
{
	// シーン切り替え中なら何もしない
	if (GetContext()->GetPISceneManager()->IsChanging())
	{
		return;
	}

	// F5でシーン移動
	if (GetContext()->GetPIInput()->GetKeyDown(KeyName::F5))
	{
		GetContext()->GetPISceneManager()->SetNextScene<GamePlayScene>();
		return;
	}

	// 次の状態に遷移
	std::unique_ptr<Systems::State<GamePlaySceneInternals>> nextState{ m_currentState->MoveNextState() };
	if (nextState)
	{
		SetState(std::move(nextState));
	}

	// 状態ごとの処理
	m_currentState->Update(&m_internals, elapsedTime);

	// シェーダにプレイヤーの座標を設定
	Renderings::ConstantBuffer* pConstantBuffer = m_internals.pGroundModel->GetPModelSource()->GetPPixelShader()->GetConstantBuffer();
	pConstantBuffer->SetVariable("ShadowPosition", m_internals.pPlayer->GetPosition() + Math::Vector3::Down * m_internals.pPlayer->GetBox().size.y / 2.0f);
	// NPC の当たり判定
	const auto nPCBoxes = m_internals.pNPCManager->GetPBoxes();
	for (size_t i = 0; i < nPCBoxes.size(); i++)
	{
		pConstantBuffer->SetVariable("ShadowPosition", nPCBoxes.at(i)->position + Math::Vector3::Down * nPCBoxes.at(i)->size.y / 2.0f, 16 * (i + 1));
	}
}

// 終了処理
void GamePlayScene::Finalize()
{
	// マウスをループ解除
	GetContext()->GetPIInput()->SetMousePositionLoop(true);
}

// メッセージを受け取る
void GamePlayScene::AcceptMessage(const std::string& message)
{
	if (message == "WindowSizeChanged")
	{
		// 出力サイズ
		const Math::Vector2& outputSize = GetContext()->GetPIWindowController()->GetOutputSize();
		// プロジェクション行列を設定
		m_internals.pCameraScreen->SetProjectionMatrix(outputSize);
		// キャンバスのサイズを設定
		m_internals.pCanvas->SetSize(outputSize);

		// UIの設定
		m_internals.pNPCManager->SetCursor(*m_internals.pCameraScreen);
		m_internals.pZ2Talk->SetPosition(m_internals.pPlayer->GetBox(), *m_internals.pCameraScreen);
		m_internals.pBlackBelt->SetWidth(m_internals.pCanvas->GetSize().x);
	}
}

// 状態を変更
void GamePlayScene::SetState(std::unique_ptr<Systems::State<GamePlaySceneInternals>> state)
{
	if (m_currentState != nullptr)
	{
		m_currentState->Exit(&m_internals);
	}
	m_currentState = std::move(state);
	m_currentState->Enter(&m_internals);
}
