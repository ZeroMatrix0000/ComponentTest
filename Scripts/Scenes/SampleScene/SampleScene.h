/*
 * FileName:     SampleScene.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/09
 *
 * サンプルシーン
 */

#pragma once

#include "Scripts/Commons/Scenes/Scene.h"

namespace Renderings
{
	class ICameraScreen;
	class Canvas;
	class ParticleManager;
}
class SceneTransitionData;
class GameContext;
class DebugCamera;

// サンプルシーン
class SampleScene : public Scenes::Scene<SceneTransitionData, GameContext>
{

	typedef Scene<SceneTransitionData, GameContext> Scene;


public:


	/* メンバ関数 */

	// コンストラクタ
	SampleScene(const ComponentDesc& desc);

	// 初期化処理
	void Initialize(const SceneTransitionData& data) override;
	// 更新処理
	void Update(float elapsedTime) override;
	// 終了処理
	void Finalize() override;

	// メッセージを受け取る
	void AcceptMessage(const std::string& message) override;


private:


	/* メンバ変数 */

	// テスト
	std::unique_ptr<GameObject> m_test3D;

	// 地面
	std::unique_ptr<GameObject> m_ground;

	// カメラ画面
	Renderings::ICameraScreen* m_pCameraScreen;
	// デバッグカメラ
	DebugCamera* m_pDebugCamera;

	// キャンバス
	Renderings::Canvas* m_pCanvas;

	// パーティクル管理
	Renderings::ParticleManager* m_pParticleManager;

};
