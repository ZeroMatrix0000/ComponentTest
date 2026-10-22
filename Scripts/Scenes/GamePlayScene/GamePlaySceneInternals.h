/*
 * FileName:     GamePlaySceneInternals.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/22
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
	GamePlaySceneInternals(Ref<std::vector<std::unique_ptr<GameObject>>> refGameObjects, Ref<const GameContext> refGameContext)
		: refGameObjects{ refGameObjects }
		, refGameContext{ refGameContext }
		, pPlayer{}
		, pPlayerCamera{}
		, pNPCManager{}
		, pGroundMesh{}
		, pGroundModel{}
		, pReticle{}
		, pZ2Talk{}
		, pBlackBelt{}
		, pMessageWindow{}
		, pTargetPoint{}
		, pCameraScreen{}
		, pCanvas{}
		, pActionManager{}
	{
	}


	/* メンバ変数 */

	// ゲームオブジェクトリスト
	Ref<std::vector<std::unique_ptr<GameObject>>> refGameObjects;
	// ゲームコンテキスト
	Ref<const GameContext> refGameContext;

	// プレイヤー
	Player* pPlayer;
	// プレイヤーカメラ
	PlayerCamera* pPlayerCamera;

	// NPC管理
	NPCManager* pNPCManager;

	// 地面
	Colliders::MeshCollider* pGroundMesh;
	Renderings::Model3D* pGroundModel;

	// レティクル
	Renderings::Image* pReticle;

	// Z: はなす
	Z2Talk* pZ2Talk;

	// 上下の黒帯
	BlackBelt* pBlackBelt;

	// メッセージウィンドウ
	MessageWindow* pMessageWindow;

	// ターゲット照準
	TargetPoint* pTargetPoint;

	// カメラ画面
	Renderings::ICameraScreen* pCameraScreen;

	// キャンバス
	Renderings::Canvas* pCanvas;

	// アクション管理
	ActionManager* pActionManager;

};
