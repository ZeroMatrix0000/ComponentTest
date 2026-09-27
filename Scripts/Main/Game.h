/*
 * FileName:     Game.h
 * Author:       Takao Hayata
 * Last Updated: 2026/07/31
 *
 * ゲーム
 */

#pragma once

#include "Scripts/Commons/Systems/OnlyOne.h"
#include "Scripts/Commons/Renderings/DeviceResources.h"
#include "Scripts/Commons/Renderings/RenderingResources.h"
#include "Scripts/Commons/Renderings/DeviceResources2D.h"
#include "Scripts/Commons/Renderings/Renderer.h"
#include "Scripts/Commons/Systems/Resources.h"
#include "Scripts/Commons/Systems/WindowController.h"
#include "Scripts/Commons/Systems/Input.h"
#include "Scripts/Commons/Systems/Timer.h"
#include "Scripts/Commons/Systems/ErrorMessage.h"
#include "Scripts/Commons/GameObjects/GameObjectManager.h"
#include "Scripts/Commons/Components/ComponentManager.h"
#include "Scripts/Commons/Scenes/SceneManager.h"
#include "SceneTransitionData.h"
#include "GameInput.h"
#include "GameContext.h"

// ゲーム
class Game : public Systems::OnlyOne
{

public:


	/* メンバ関数 */

	// コンストラクタ
	Game();

	// 初期化処理
	void Initialize(const HWND& hWindow);
	// 更新処理
	void Update();
	// 描画処理
	void Render();
	// 終了処理
	void Finalize();

	// ウィンドウサイズ変更時の処理
	void OnWindowSizeChanged(const Math::Vector2Int& outputSize);

	// 1F間の経過時間を取得
	float GetDeltaTime() const { return m_timer.GetDeltaTime(); }

private:


	/* メンバ関数 */

	// 対応付け
	void RegisterComponents();


	/* メンバ変数 */

	// ウィンドウハンドル
	HWND m_hWindow;

	// デバイスリソース
	Renderings::DeviceResources m_deviceResources;
	// 描画リソース
	Renderings::RenderingResources m_renderingResources;
	// 2Dデバイスリソース
	Renderings::DeviceResources2D m_deviceResources2D;

	// 描画
	Renderings::Renderer m_renderer;

	// リソース
	Systems::Resources m_resources;
	// タイマー
	Systems::Timer m_timer;

	// ウィンドウ管理
	Systems::WindowController m_windowController;
	// 入力管理
	Systems::Input m_input;

	// エラーメッセージ
	Systems::ErrorMessage m_errorMessage;

	// コンポーネント管理
	ComponentManager m_componentManager;
	// ゲームオブジェクト管理
	GameObjectManager m_gameObjectManager;

	// シーン管理
	Scenes::SceneManager<SceneTransitionData, GameContext> m_sceneManager;

	// ゲーム入力
	GameInput m_gameInput;

	// ゲームコンテキスト
	GameContext m_context;

};