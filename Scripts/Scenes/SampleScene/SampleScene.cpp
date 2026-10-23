/*
 * FileName:     SampleScene.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/23
 *
 * サンプルシーン
 */

#include "Pch.h"
#include "SampleScene.h"

#include "Scripts/Main/GameContext.h"
#include "Scripts/Main/IsDerivedRegister.h"
#include "Scripts/Commons/Systems/IWindowController.h"
#include "Scripts/Commons/Systems/IInput.h"
#include "Scripts/Commons/Scenes/ISceneManager.h"
#include "Scripts/Commons/GameObjects/GameObject.h"
#include "Scripts/Commons/GameObjects/IGameObjectManager.h"
#include "Scripts/Commons/Renderings/ICameraScreen.h"
#include "Scripts/Commons/Renderings/Canvas.h"
#include "Scripts/Commons/Renderings/Particle/ParticleManager.h"
#include "Scripts/Commons/Renderings/Particle/ParticleSample.h"
#include "Scripts/Commons/Colliders/BoxCollider.h"
#include "Scripts/Commons/Colliders/SphereCollider.h"
#include "Scripts/Commons/Colliders/MeshCollider.h"
#include "Scripts/GameObjects/Objects/DebugCamera/DebugCamera.h"

// コンストラクタ
SampleScene::SampleScene(const ComponentDesc& desc)
	: Scene{ desc }
	, m_test3D{}
	, m_ground{}
	, m_pCameraScreen{}
	, m_pDebugCamera{}
	, m_pCanvas{}
	, m_pParticleManager{}
{
}

// 初期化処理
void SampleScene::Initialize(const SceneTransitionData& data)
{
	// コンテキスト
	Ref gameContext = GetContext();

	// 出力サイズ
	const Math::Vector2& outputSize = gameContext->GetPIWindowController()->GetOutputSize();

	// ゲームオブジェクト管理
	auto* pIGameObjectManager = gameContext->GetPIGameObjectManager();

	pIGameObjectManager->SetGameObjectsRef(GetGameObjects());
	pIGameObjectManager->Load("Scene_Sample");

	// デバッグカメラを取得
	Ref<GameObject> camera = pIGameObjectManager->Find("DebugCamera");
	m_pCameraScreen = &camera->GetComponent<Renderings::ICameraScreen>().Get();
	m_pCameraScreen->SetProjectionMatrix(outputSize);
	m_pDebugCamera = &camera->GetComponent<DebugCamera>().Get();

	// キャンバスを取得
	m_pCanvas = &pIGameObjectManager->Find("Canvas")->GetComponent<Renderings::Canvas>().Get();
	m_pCanvas->SetSize(outputSize);

	// パーティクル管理を取得
	m_pParticleManager = &pIGameObjectManager->Find("ParticleManager")->GetComponent<Renderings::ParticleManager>().Get();

	// プレイヤーを取得
	Ref<GameObject> player = pIGameObjectManager->Find("Player");
	player->GetComponent<Colliders::BoxCollider>()->ApplyTransform();
	player->GetComponent<Colliders::SphereCollider>()->ApplyTransform();

	// 地面を取得
	Ref<GameObject> ground = pIGameObjectManager->Find("Ground");
	ground->GetComponent<Colliders::MeshCollider>()->ApplyTransform();
}

// 更新処理
void SampleScene::Update(float elapsedTime)
{
	// 入力管理
	auto* pIInput = GetContext()->GetPIInput();

	// F5でシーン移動
	if (pIInput->GetKeyDown(KeyName::F5))
	{
		GetContext()->GetPISceneManager()->SetNextScene<SampleScene>();
		return;
	}

	if (pIInput->GetKey(KeyName::Space))
	{
		m_pParticleManager->Play<Renderings::ParticleSample>();
	}
	m_pParticleManager->Update(elapsedTime);

	// カメラの更新
	m_pDebugCamera->SetInput
	(
		pIInput->GetMouseMovement(),
		pIInput->GetMouseButton(MouseButtonName::Right),
		pIInput->GetMouseButton(MouseButtonName::Middle),
		pIInput->GetMouseWheelDelta()
	);
	m_pDebugCamera->Update(elapsedTime);
	m_pCameraScreen->UpdateViewMatrix();
}

// 終了処理
void SampleScene::Finalize()
{
}

// メッセージを受け取る
void SampleScene::AcceptMessage(const std::string& message)
{
	if (message == "WindowSizeChanged")
	{
		// 出力サイズ
		const Math::Vector2& outputSize = GetContext()->GetPIWindowController()->GetOutputSize();
		// プロジェクション行列を設定
		m_pCameraScreen->SetProjectionMatrix(outputSize);
		// キャンバスのサイズを設定
		m_pCanvas->SetSize(outputSize);
	}
}
