/*
 * FileName:     Game.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/13
 *
 * ゲーム
 */

#include "Pch.h"
#include "Game.h"

#include "ComponentsRegisterer.h"
#include "Scripts/Scenes/Scenes.h"

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
	, m_componentManager{ Ref<IGameObjectInstantiator>{ m_gameObjectManager } }
	, m_gameObjectManager{ &m_resources }
	, m_sceneManager{ Ref{ m_context }.Const() }
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

	// リソースの初期化
	InitializeResources();

	// 描画の初期化
	m_renderer.Initialize(device, context, swapChain, commonStates);
	// テキスト描画を初期化
	m_renderer.GetTextRenderer().Initialize
	(
		m_deviceResources2D.GetDevice(),
		m_deviceResources2D.GetContext(),
		m_deviceResources2D.GetBackBuffer()
	);
	m_renderer.GetModelRenderer().SetDepthStencilView(m_deviceResources.GetDepthStencilView());
	m_renderer.GetModelRenderer().SetVertexShader(m_resources.GetVertexShader("InstancingTest"));
	m_renderer.GetModelRenderer().SetPixelShader(m_resources.GetPixelShader("InstancingTest"));
	m_renderer.GetTextRenderer().CreateFontCollection(L"Resources\\Fonts");

	// タイマーの初期化
	m_timer.Initialize();

	// ウィンドウ管理の初期化
	m_windowController.Initialize(hWindow);
	// 入力の初期化
	m_input.Initialize(hWindow);

	// ゲームオブジェクト管理の初期化
	m_gameObjectManager.Initialize(&m_componentManager);

	// コンポーネントの登録
	ComponentsRegisterer::RegisterComponents
	(
		Ref{ m_componentManager },
		Ref{ m_gameObjectManager },
		Ref<Renderings::IModel3DRenderer>{ m_renderer.GetModelRenderer() },
		Ref<Renderings::IImageRenderer>{ m_renderer.GetImageRenderer() },
		Ref<Renderings::ITextRenderer>{ m_renderer.GetTextRenderer() },
		Ref<Renderings::IColliderRenderer>{ m_renderer.GetColliderRenderer() },
		Ref<Systems::IResources>{ m_resources }
	);

	// エラーメッセージのオブジェクトを生成
	m_errorMessage.CreateObjects(&m_gameObjectManager);

	// シーン管理の初期化
	InitializeSceneManager();

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

	Scenes::Scene<SceneTransitionData, GameContext>::SetContext(Ref{ m_context }.Const());

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
	m_deviceResources.Present(false);
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

	m_renderer.GetModelRenderer().SetDepthStencilView(m_deviceResources.GetDepthStencilView());
	m_renderer.GetTextRenderer().OnWindowSizeChanged(m_deviceResources2D.GetContext(), m_deviceResources2D.GetBackBuffer());

	m_sceneManager.OnWindowSizeChanged(outputSize);
}

// リソースの初期化
void Game::InitializeResources()
{
	// デバイス
	auto* device = m_deviceResources.GetD3DDevice();

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
	// 頂点シェーダの読み込み
	m_resources.LoadVertexShaders(device, L"Resources\\Shaders\\VertexShaders");
	// ピクセルシェーダの読み込み
	m_resources.LoadPixelShaders(device, L"Resources\\Shaders\\PixelShaders");

	// シェーダを適用
	m_resources.GetEditableModelSource("NewGrounds")->SetPixelShader(m_resources.GetPixelShader("PlayerShadow"));
	m_resources.GetEditableModelSource("PlayerSilhouette")->SetPixelShader(m_resources.GetPixelShader("PlayerSilhouette"));

	// 描画順を設定
	m_resources.GetEditableModelSource("PlayerSilhouette")->SetDrawingOrder(1000);
	m_resources.GetEditableModelSource("Player")->SetDrawingOrder(2000);
	m_resources.GetEditableModelSource("NPC")->SetDrawingOrder(2000);
	m_resources.GetEditableModelSource("Box")->SetDrawingOrder(2000);

	// 深度ステンシルステートの詳細
	D3D11_DEPTH_STENCIL_DESC desc = {};
	// 深度テスト
	desc.DepthEnable = true;
	// 深度を変更しない
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	// 深度が大きければ描画
	desc.DepthFunc = D3D11_COMPARISON_GREATER;
	// ステンシルテスト
	desc.StencilEnable = true;
	desc.StencilReadMask = 0xFF;
	desc.StencilWriteMask = 0xFF;

	// ステンシル操作の詳細
	D3D11_DEPTH_STENCILOP_DESC stencilOp = {};
	// ステンシル値が 0 の場合のみ描画
	stencilOp.StencilFunc = D3D11_COMPARISON_EQUAL;
	// 描画成功時に 値を 増やす
	stencilOp.StencilPassOp = D3D11_STENCIL_OP_INCR;
	// 描画失敗時は何もしない
	stencilOp.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	stencilOp.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

	desc.FrontFace = stencilOp;
	desc.BackFace = stencilOp;

	// 深度ステンシルステートを設定
	m_resources.GetEditableModelSource("PlayerSilhouette")->SetDepthStencilState(desc, device);
}

// シーン管理の初期化
void Game::InitializeSceneManager()
{
	// コンポーネント管理のインタフェース
	Ref iComponentManager = Ref<IComponentManager>{ m_componentManager };
	// 未参照オブジェクト
	Ref nullReference = m_gameObjectManager.GetNullReference();

	// シーンの追加
	m_sceneManager.AddScene<SampleScene>([iComponentManager, nullReference]
		{
			return std::unique_ptr<SampleScene>{ static_cast<SampleScene*>(iComponentManager->Create<SampleScene>(nullReference).release()) };
		});
	m_sceneManager.AddScene<TitleScene>([iComponentManager, nullReference]
		{
			return std::unique_ptr<TitleScene>{ static_cast<TitleScene*>(iComponentManager->Create<TitleScene>(nullReference).release()) };
		});
	m_sceneManager.AddScene<GamePlayScene>([iComponentManager, nullReference]
		{
			return std::unique_ptr<GamePlayScene>{ static_cast<GamePlayScene*>(iComponentManager->Create<GamePlayScene>(nullReference).release()) };
		});
	// シーン管理の初期化
	m_sceneManager.Initialize(m_windowController.GetOutputSize(), &m_gameObjectManager);
}
