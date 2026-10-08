/*
 * FileName:     TitleScene.h
 * Author:       Takao Hayata
 * Last Updated: 2026/09/28
 *
 * タイトルシーン
 */

#include "Pch.h"
#include "TitleScene.h"

#include "../Scenes.h"
#include "Scripts/Commons/Systems/IWindowController.h"
#include "Scripts/Commons/Systems/IInput.h"
#include "Scripts/Commons/Scenes/ISceneManager.h"
#include "Scripts/Commons/Renderings/Image/Image.h"
#include "Scripts/Commons/Renderings/Shader/PixelShader.h"
#include "Scripts/Commons/Renderings/Canvas.h"
#include "Scripts/Commons/Renderings/CameraScreen.h"
#include "Scripts/Commons/Components/RectTransform.h"
#include "Scripts/Commons/GameObjects/GameObject.h"
#include "Scripts/Commons/GameObjects/IGameObjectManager.h"
#include "Scripts/Commons/Colliders/MeshCollider.h"
#include "Scripts/Main/GameContext.h"
#include "Scripts/GameObjects/UIs/SelectMenu/SelectMenu.h"

// コンストラクタ
TitleScene::TitleScene(const ComponentDesc& desc)
	: Scene{ desc }
	, m_pCameraScreen{}
	, m_pCanvas{}
	, m_pSelectMenu{}
	, m_pTitle{}
	, m_pTitleImage{}
	, m_titlePosition{}
	, m_titleSway{}
{
}

// 初期化処理
void TitleScene::Initialize(const SceneTransitionData& data)
{
	// コンテキスト
	const auto& gameContext = GetContext();

	// 出力サイズ
	const Math::Vector2& outputSize = gameContext.GetPIWindowController()->GetOutputSize();

	// ゲームオブジェクト管理
	auto* pIGameObjectManager = gameContext.GetPIGameObjectManager();

	pIGameObjectManager->SetPGameObjects(GetPGameObjects());
	pIGameObjectManager->Load("Scene_Title");

	// カメラ画面を取得
	m_pCameraScreen = pIGameObjectManager->Find("TitleCamera")->GetComponent<Renderings::CameraScreen<Camera::EulerTargetCamera>>();
	m_pCameraScreen->SetProjectionMatrix(outputSize);

	// キャンバスを取得
	m_pCanvas = pIGameObjectManager->Find("Canvas")->GetComponent<Renderings::Canvas>();
	m_pCanvas->SetSize(outputSize);

	// 地面を取得
	pIGameObjectManager->Find("Ground")->GetComponent<Colliders::MeshCollider>()->ApplyTransform();

	// 選択メニューを取得
	m_pSelectMenu = pIGameObjectManager->Find("TitleMenu")->GetComponent<SelectMenu>();
	m_pSelectMenu->AddOption(L"スタート", [&] { gameContext.GetPISceneManager()->SetNextScene<GamePlayScene>(); });
	m_pSelectMenu->AddOption(L"ゲームを<ruby=しゅうりょう>終了</ruby>", [&] { gameContext.GetPIWindowController()->Destroy(); });

	// タイトルを取得
	GameObject* pObj = pIGameObjectManager->Find("Title");
	m_pTitle = pObj->GetComponent<RectTransform>();
	m_pTitleImage = pObj->GetComponent<Renderings::Image>();
	m_pTitleImage->GetPPixelShader()->GetConstantBuffer()->SetVariable("TextureSize", m_pTitleImage->GetSize());
	m_titlePosition = m_pTitle->GetRect().position;
	m_titleSway.Initialize(0.0f, 0.0f, 360.0f);
}

// 更新処理
void TitleScene::Update(float elapsedTime)
{
	// シーン切り替え中でないなら
	if (!GetContext().GetPISceneManager()->IsChanging())
	{
		// 入力管理
		auto* pIInput = GetContext().GetPIInput();

		// W or ↑ で上選択
		if (pIInput->GetKeyDown(KeyName::W) || pIInput->GetKeyDown(KeyName::Up))
		{
			m_pSelectMenu->SelectUp();
		}
		// S or ↓ で下選択
		if (pIInput->GetKeyDown(KeyName::S) || pIInput->GetKeyDown(KeyName::Down))
		{
			m_pSelectMenu->SelectDown();
		}
		// Z or Space で実行
		if (pIInput->GetKeyDown(KeyName::Z) || pIInput->GetKeyDown(KeyName::Space))
		{
			m_pSelectMenu->Execute();
		}
	}

	// 選択メニューの更新
	m_pSelectMenu->Update(elapsedTime);

	// タイトルの更新
	m_titleSway += 90.0f * elapsedTime;
	m_pTitle->SetPosition(m_titlePosition + Math::Vector2::UnitY * Math::Sin(Math::Deg2Rad(m_titleSway)) * 50.0f);
	auto* buffer = m_pTitleImage->GetPPixelShader()->GetConstantBuffer();
	buffer->SetVariable("Time", buffer->GetVariable<float>("Time") + elapsedTime);
	//buffer->SetVariable("timer", buffer->GetVariable<float>("time") + elapsedTime);

	// カメラ画面の更新
	Camera::EulerTargetCamera camera = m_pCameraScreen->GetCamera();
	camera.rotation.y += 30.0f * elapsedTime;
	camera.rotation.Format();
	m_pCameraScreen->SetCamera(camera);
	m_pCameraScreen->UpdateViewMatrix();
}

// 終了処理
void TitleScene::Finalize()
{
}

// メッセージを受け取る
void TitleScene::AcceptMessage(const std::string& message)
{
	if (message == "WindowSizeChanged")
	{
		// 出力サイズ
		const Math::Vector2& outputSize = GetContext().GetPIWindowController()->GetOutputSize();
		// プロジェクション行列を設定
		m_pCameraScreen->SetProjectionMatrix(outputSize);
		// キャンバスのサイズを設定
		m_pCanvas->SetSize(outputSize);
	}
}
