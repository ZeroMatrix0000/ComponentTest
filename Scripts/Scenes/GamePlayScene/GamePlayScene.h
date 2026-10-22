/*
 * FileName:     GamePlayScene.h
 * Author:       Takao Hayata
 * Last Updated: 2026/09/06
 *
 * ゲームプレイシーン
 */

#pragma once

#include "GamePlaySceneInternals.h"
#include "Scripts/Commons/Scenes/Scene.h"
#include "GamePlaySceneInitializer.h"

namespace Renderings
{
	class ICameraScreen;
	class Canvas;
}
namespace Colliders
{
	class MeshCollider;
}
namespace Systems
{
	template<typename TInternals>
	class State;
}
class SceneTransitionData;
class GameContext;
class Player;
class PlayerCamera;
class NPCManager;

// ゲームプレイシーン
class GamePlayScene : public Scenes::Scene<SceneTransitionData, GameContext>
{

	typedef Scene<SceneTransitionData, GameContext> Scene;


public:


	/* メンバ関数 */

	// コンストラクタ
	GamePlayScene(const ComponentDesc& desc);

	// 初期化処理
	void Initialize(const SceneTransitionData& data) override;
	// 更新処理
	void Update(float elapsedTime) override;
	// 終了処理
	void Finalize() override;

	// メッセージを受け取る
	void AcceptMessage(const std::string& message) override;


private:


	/* メンバ関数 */

	// 状態を変更
	void SetState(std::unique_ptr<Systems::State<GamePlaySceneInternals>> state);


	/* メンバ変数 */

	// 内部データ
	GamePlaySceneInternals m_internals;

	// 状態
	std::unique_ptr<Systems::State<GamePlaySceneInternals>> m_currentState;

	// 初期化関数
	GamePlaySceneInitializer m_Initializer;

};
