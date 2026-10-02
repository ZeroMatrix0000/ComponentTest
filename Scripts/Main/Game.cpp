/*
 * FileName:     Game.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/09/28
 *
 * ゲーム
 */

#include "Pch.h"
#include "Game.h"

#include "Scripts/Scenes/Scenes.h"
#include "Scripts/Scenes/GamePlayScene/Actions/Actions.h"
#include "Scripts/Scenes/GamePlayScene/Actions/ActionManager.h"
#include "Scripts/GameObjects/UIs/SelectMenu/SelectMenu.h"
#include "Scripts/GameObjects/UIs/InteractCursor/InteractCursor.h"
#include "Scripts/GameObjects/UIs/Z2Talk/Z2Talk.h"
#include "Scripts/GameObjects/UIs/BlackBelt/BlackBelt.h"
#include "Scripts/GameObjects/UIs/MessageWindow/MessageWindow.h"
#include "Scripts/GameObjects/Objects/DebugCamera/DebugCamera.h"
#include "Scripts/GameObjects/Objects/Player/Player.h"
#include "Scripts/GameObjects/Objects/Player/PlayerModel.h"
#include "Scripts/GameObjects/Objects/Player/PlayerCamera.h"
#include "Scripts/GameObjects/Objects/NPC/NPC.h"
#include "Scripts/GameObjects/Objects/NPC/NPCManager.h"
#include "Scripts/Commons/Renderings/Model3D/Model3D.h"
#include "Scripts/Commons/Renderings/Image/Image.h"
#include "Scripts/Commons/Renderings/Text/Text.h"
#include "Scripts/Commons/Renderings/Canvas.h"
#include "Scripts/Commons/Renderings/CameraScreen.h"
#include "Scripts/Commons/Colliders/BoxCollider.h"
#include "Scripts/Commons/Colliders/SphereCollider.h"
#include "Scripts/Commons/Colliders/MeshCollider.h"
#include "Scripts/Commons/Components/Transform.h"
#include "Scripts/Commons/Components/RectTransform.h"

// コンストラクタ
Game::Game()
	: OnlyOne{ typeid(Game) }
	, m_hWindow{}
	, m_deviceResources{}
	, m_renderingResources{}
	, m_deviceResources2D{ m_deviceResources }
	, m_renderer{}
	, m_resources{}
	, m_timer{}
	, m_windowController{}
	, m_input{}
	, m_errorMessage{}
	, m_componentManager{}
	, m_gameObjectManager{ m_resources }
	, m_sceneManager{ m_context }
	, m_gameInput{ m_input }
	, m_context{}
{
}

// 初期化処理
void Game::Initialize(const HWND& hWindow)
{
	// ウィンドウハンドルを保持
	m_hWindow = hWindow;

	// エラーメッセージの初期化
	m_errorMessage.Initialize(5.0f, 20, true);

	// デバイスリソースの初期化
	m_deviceResources.Initialize(hWindow);
	m_deviceResources2D.Initialize();

	// デバイス
	auto* device = m_deviceResources.GetD3DDevice();
	// コンテキスト
	auto* context = m_deviceResources.GetD3DContext();
	// スワップチェイン
	auto* swapChain = m_deviceResources.GetSwapChain();

	// 描画リソースの初期化
	m_renderingResources.Initialize(device);
	m_renderingResources.SetEffectFactoryDirectory(L"Resources\\Models");

	// コモンステート
	const auto& commonStates = m_renderingResources.GetCommonStates();
	// エフェクトファクトリー
	auto* fx = m_renderingResources.GetEffectFactory();

	// 画像の読み込み
	m_resources.LoadImageSources(device, L"Resources\\Images");
	// モデルソースの読み込み
	m_resources.LoadModelSources(device, fx, L"Resources\\Models");
	// Jsonの読み込み
	m_resources.LoadJsons(L"Resources\\Jsons");
	m_resources.AddJsonFromStr
	(
		"Prefab_ErrorMessage",
		R"(
{
	"Text": {
		"Str": "",
		"FontName": "GenEi M Gothic v2",
		"FontSize": 24.0,
		"FontColor": {
			"R": 1.0,
			"G": 1.0,
			"B": 0.0,
			"A": 1.0
		},
		"OutlineWidth": 2.0,
		"OrderInLayer": 65535
	},
	"RectTransform": {
		"Position": {
			"X": 10.0,
			"Y": 10.0
		},
		"Size": {
			"X": 100000.0,
			"Y": 100000.0
		},
		"Pivot": "TopLeft",
		"Anchor": "TopLeft"
	}
}
		)"
	);
	m_resources.AddJsonFromStr
	(
		"Prefab_Canvas",
		R"(
{
	"Canvas": {
	}
}
		)"
	);
	m_resources.AddJsonFromStr
	(
		"Prefab_SceneTransitionAnimation",
		R"(
{
	"Image": {
		"ImageSourceName": "Box",
		"Color": {
			"R": 0.0,
			"G": 0.0,
			"B": 0.0,
			"A": 1.0
		},
		"OrderInLayer": 32767
	},
	"RectTransform": {
		"Angle": 15.0
	}
}
		)"
	);
	// メッシュの読み込み
	m_resources.LoadMeshes(L"Resources\\Meshes");
	// ピクセルシェーダの読み込み
	m_resources.LoadPixelShaders(device, L"Resources\\Shaders\\PixelShaders");

	// 描画の初期化
	m_renderer.Initialize(device, context, swapChain, commonStates);
	// テキスト描画を初期化
	m_renderer.GetTextRenderer().Initialize
	(
		m_deviceResources2D.GetDevice(),
		m_deviceResources2D.GetContext(),
		m_deviceResources2D.GetBackBuffer()
	);
	m_renderer.GetTextRenderer().CreateFontCollection(L"Resources\\Fonts");

	// タイマーの初期化
	m_timer.Initialize();

	// ウィンドウ管理の初期化
	m_windowController.Initialize(hWindow);
	// 入力の初期化
	m_input.Initialize(hWindow);

	// コンポーネント管理の初期化
	m_componentManager.Initialize(&m_gameObjectManager);
	// ゲームオブジェクト管理の初期化
	m_gameObjectManager.Initialize(&m_componentManager);

	// 対応付け
	RegisterComponents();

	// コンポーネント管理のインタフェース
	IComponentManager* pIComponentManager = &m_componentManager;

	// エラーメッセージのオブジェクトを生成
	m_errorMessage.CreateObjects(&m_gameObjectManager);

	// シーンの追加
	m_sceneManager.AddScene<SampleScene>([pIComponentManager]
	{
		return std::unique_ptr<SampleScene>{ static_cast<SampleScene*>(pIComponentManager->Create<SampleScene>().release()) };
	});
	m_sceneManager.AddScene<TitleScene>([pIComponentManager]
	{
		return std::unique_ptr<TitleScene>{ static_cast<TitleScene*>(pIComponentManager->Create<TitleScene>().release()) };
	});
	m_sceneManager.AddScene<GamePlayScene>([pIComponentManager]
	{
		return std::unique_ptr<GamePlayScene>{ static_cast<GamePlayScene*>(pIComponentManager->Create<GamePlayScene>().release()) };
	});
	// シーン管理の初期化
	m_sceneManager.Initialize(m_windowController.GetOutputSize(), &m_gameObjectManager);

	// コンテキストの初期化
	m_context.Initialize
	(
		&m_windowController,
		&m_input,
		&m_componentManager,
		&m_gameObjectManager,
		&m_sceneManager,
		&m_gameInput
	);

	// 最初のシーンを設定
	//m_sceneManager.SetFirstScene<SampleScene>();
	m_sceneManager.SetFirstScene<TitleScene>();
}

// 更新処理
void Game::Update()
{
	// タイマーの更新
	m_timer.Update();

	// 入力の更新
	m_input.Update();

	// F4でフルスクリーン表示切り替え
	if (m_input.GetKeyDown(KeyName::F4))
	{
		m_windowController.ChangeFullScreen();
	}
	// F3で描画モード切り替え
	if (m_input.GetKeyDown(KeyName::F3))
	{
		m_renderer.ChangeRenderMode();
	}

	// シーン管理の更新
	m_sceneManager.Update(m_timer.GetDeltaTime());

	// エラーメッセージの更新
	m_errorMessage.Update(m_timer.GetDeltaTime());
}

// 描画処理
void Game::Render()
{
	// ウィンドウを受け取っていないなら何もしない
	if (!m_hWindow)
	{
		return;
	}

	// 画面の初期化
	m_deviceResources.Clear();

	// 描画
	m_renderer.Render();

	// 画面の表示
	m_deviceResources.Present(true);
}

// 終了処理
void Game::Finalize()
{
	// シーン管理の終了
	m_sceneManager.Finalize();
}

// ウィンドウサイズ変更時の処理
void Game::OnWindowSizeChanged(const Math::Vector2Int& outputSize)
{
	// ウィンドウを受け取っていないなら何もしない
	if (!m_hWindow)
	{
		return;
	}

	// ウィンドウの大きさの幅か高さが0なら何もしない
	if (outputSize.x == 0.0f || outputSize.y == 0.0f)
	{
		return;
	}

	// 2Dリソースをリセット
	m_deviceResources2D.Reset();

	m_deviceResources.OnWindowSizeChanged(outputSize);
	m_windowController.SetOutputSize(outputSize);

	m_deviceResources2D.OnWindowSizeChanged();

	// デバイス
	auto* device = m_deviceResources.GetD3DDevice();
	// コンテキスト
	auto* context = m_deviceResources.GetD3DContext();
	// スワップチェイン
	auto* swapChain = m_deviceResources.GetSwapChain();

	m_renderer.GetTextRenderer().OnWindowSizeChanged(m_deviceResources2D.GetContext(), m_deviceResources2D.GetBackBuffer());

	m_sceneManager.OnWindowSizeChanged(outputSize);
}

// 対応付け
void Game::RegisterComponents()
{
	// 3Dモデル
	m_componentManager.RegisterCreate<Renderings::Model3D>([&](const ComponentDesc& desc)
	{
		return std::make_unique<Renderings::Model3D>(desc, &m_renderer.GetModelRenderer(), m_resources);
	});
	// 画像
	m_componentManager.RegisterCreate<Renderings::Image>([&](const ComponentDesc& desc)
	{
		return std::make_unique<Renderings::Image>(desc, &m_renderer.GetImageRenderer(), m_resources);
	});
	// テキスト
	m_componentManager.RegisterCreate<Renderings::Text>([&](const ComponentDesc& desc)
	{
		return std::make_unique<Renderings::Text>(desc, &m_renderer.GetTextRenderer());
	});

	// 長方形の当たり判定
	m_componentManager.RegisterCreate<Colliders::BoxCollider>([&](const ComponentDesc& desc)
	{
		return std::make_unique<Colliders::BoxCollider>(desc, &m_renderer.GetColliderRenderer());
	});
	// 球の当たり判定
	m_componentManager.RegisterCreate<Colliders::SphereCollider>([&](const ComponentDesc& desc)
	{
		return std::make_unique<Colliders::SphereCollider>(desc, &m_renderer.GetColliderRenderer());
	});
	// メッシュの当たり判定
	m_componentManager.RegisterCreate<Colliders::MeshCollider>([&](const ComponentDesc& desc)
	{
		return std::make_unique<Colliders::MeshCollider>(desc, &m_renderer.GetColliderRenderer(), m_resources);
	});

	// カメラ画面
	m_componentManager.RegisterCreate<Renderings::ICameraScreen>([&](const ComponentDesc& desc)
	{
		return std::make_unique<Renderings::CameraScreen<Camera::QuaternionCamera>>(desc);
	});

	// アクション管理
	m_componentManager.RegisterCreate<ActionManager>([&](const ComponentDesc& desc)
	{
		return std::make_unique<ActionManager>(desc, m_resources);
	});


	// トランスフォーム
	m_gameObjectManager.Register<Transform>("Transform");
	// 2D用トランスフォーム
	m_gameObjectManager.Register<RectTransform>("RectTransform");
	// 3Dモデル
	m_gameObjectManager.Register<Renderings::Model3D>("Model3D");
	// カメラ画面
	m_gameObjectManager.Register<Renderings::CameraScreen<Camera::QuaternionCamera>>("QuaternionCameraScreen");
	m_gameObjectManager.Register<Renderings::CameraScreen<Camera::QuaternionTargetCamera>>("QuaternionTargetCameraScreen");
	m_gameObjectManager.Register<Renderings::CameraScreen<Camera::EulerCamera>>("EulerCameraScreen");
	m_gameObjectManager.Register<Renderings::CameraScreen<Camera::EulerTargetCamera>>("EulerTargetCameraScreen");
	// 画像
	m_gameObjectManager.Register<Renderings::Image>("Image");
	// 文字
	m_gameObjectManager.Register<Renderings::Text>("Text");
	// キャンバス
	m_gameObjectManager.Register<Renderings::Canvas>("Canvas");
	// コライダー
	m_gameObjectManager.Register<Colliders::BoxCollider>("BoxCollider");
	m_gameObjectManager.Register<Colliders::SphereCollider>("SphereCollider");
	m_gameObjectManager.Register<Colliders::MeshCollider>("MeshCollider");
	// アクション
	m_gameObjectManager.Register<Action>("Action");
	m_gameObjectManager.Register<ActionOpenMessageWindow>("ActionOpenMessageWindow");
	m_gameObjectManager.Register<ActionCloseMessageWindow>("ActionCloseMessageWindow");
	m_gameObjectManager.Register<ActionSetMessageWindowText>("ActionSetMessageWindowText");
	// アクション管理
	m_gameObjectManager.Register<ActionManager>("ActionManager");
	// 選択メニュー
	m_gameObjectManager.Register<SelectMenu>("SelectMenu");
	// カーソル
	m_gameObjectManager.Register<InteractCursor>("InteractCursor");
	// Z: はなす
	m_gameObjectManager.Register<Z2Talk>("Z2Talk");
	// 上下の黒帯
	m_gameObjectManager.Register<BlackBelt>("BlackBelt");
	// メッセージウィンドウ
	m_gameObjectManager.Register<MessageWindow>("MessageWindow");
	// デバッグ用カメラ
	m_gameObjectManager.Register<DebugCamera>("DebugCamera");
	// プレイヤー
	m_gameObjectManager.Register<Player>("Player");
	m_gameObjectManager.Register<PlayerModel>("PlayerModel");
	m_gameObjectManager.Register<PlayerCamera>("PlayerCamera");
	// NPC管理
	m_gameObjectManager.Register<NPC>("NPC");
	m_gameObjectManager.Register<NPCManager>("NPCManager");
}
