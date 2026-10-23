/*
 * FileName:     GamePlaySceneInternals.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/23
 *
 * ゲームプレイシーンの内部データ
 */

#pragma once

#include "Scripts/Commons/Systems/State.h"

namespace GameObjects
{
	class GameObject;
}
namespace Colliders
{
	class MeshCollider;
}
namespace Renderings
{
	class Model3D;
	class Image;
	class ICameraScreen;
	class Canvas;
}
class GameContext;
class Player;
class PlayerCamera;
class NPCManager;
class Z2Talk;
class BlackBelt;
class MessageWindow;
class TargetPoint;
class ActionManager;

// ゲームプレイシーンの内部データ
struct GamePlaySceneInternals
{

	/* メンバ関数 */

	// コンストラクタ
	GamePlaySceneInternals
	(
		Ref<std::vector<std::unique_ptr<GameObject>>> gameObjectsRef,
		Ref<const GameContext> gameContextRef,
		Ref<GameObject> ownerRef
	);


	/* メンバ変数 */

	// ゲームオブジェクトリストの参照
	Ref<std::vector<std::unique_ptr<GameObject>>> gameObjectsRef;
	// ゲームコンテキストの参照
	Ref<const GameContext> gameContextRef;
	// 所有者の参照
	Ref<GameObject> ownerRef;

	// プレイヤー
	Ref<Player> playerRef;
	// プレイヤーカメラ
	Ref<PlayerCamera> playerCameraRef;

	// NPC管理
	Ref<NPCManager> nPCManagerRef;

	// 地面
	Ref<Colliders::MeshCollider> groundMeshRef;
	Ref<Renderings::Model3D> groundModelRef;

	// レティクル
	Ref<Renderings::Image> reticleRef;

	// Z: はなす
	Ref<Z2Talk> z2TalkRef;

	// 上下の黒帯
	Ref<BlackBelt> blackBeltRef;

	// メッセージウィンドウ
	Ref<MessageWindow> messageWindowRef;

	// ターゲット照準
	Ref<TargetPoint> targetPointRef;

	// カメラ画面
	Ref<Renderings::ICameraScreen> iCameraScreenRef;

	// キャンバス
	Ref<Renderings::Canvas> canvasRef;

	// アクション管理
	Ref<ActionManager> actionManagerRef;

};
