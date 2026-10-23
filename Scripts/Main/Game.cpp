/*
 * FileName:     Game.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/23
 *
 * ゲーム
 */

#include "Pch.h"
#include "Game.h"

#include "ResourcesInitializer.h"
#include "ComponentsRegisterer.h"
#include "IsDerivedRegister.h"
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
	, m_gameObjectManager{ Ref<IComponentManager>{ m_componentManager }, Ref<Systems::IResources>{ m_resources } }
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
	ResourcesInitializer::InitializeResources
	(
		Ref{ m_deviceResources },
		Ref{ m_renderingResources },
		Ref{ m_resources }
	);

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
