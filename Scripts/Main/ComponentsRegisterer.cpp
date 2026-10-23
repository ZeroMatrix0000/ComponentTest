/*
 * FileName:     ComponentsRegisterer.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/23
 *
 * コンポーネント登録者
 */

#include "Pch.h"
#include "ComponentsRegisterer.h"

#include "../GameObjects/Objects/DebugCamera/DebugCamera.h"
#include "../GameObjects/Objects/Player/Player.h"
#include "../GameObjects/Objects/Player/PlayerModel.h"
#include "../GameObjects/Objects/Player/PlayerWire.h"
#include "../GameObjects/Objects/Player/PlayerCamera.h"
#include "../GameObjects/Objects/NPC/NPC.h"
#include "../GameObjects/Objects/NPC/NPCManager.h"
#include "../GameObjects/UIs/SelectMenu/SelectMenu.h"
#include "../GameObjects/UIs/InteractCursor/InteractCursor.h"
#include "../GameObjects/UIs/Z2Talk/Z2Talk.h"
#include "../GameObjects/UIs/BlackBelt/BlackBelt.h"
#include "../GameObjects/UIs/MessageWindow/MessageWindow.h"
#include "../GameObjects/UIs/TargetPoint/TargetPoint.h"
#include "../Scenes/Scenes.h"
#include "../Scenes/GamePlayScene/Actions/Actions.h"
#include "../Scenes/GamePlayScene/Actions/ActionManager.h"
#include "IsDerivedRegister.h"
#include "../Commons/Components/Transform.h"
#include "../Commons/Components/RectTransform.h"
#include "../Commons/Components/ComponentManager.h"
#include "../Commons/GameObjects/GameObjectManager.h"
#include "../Commons/Colliders/BoxCollider.h"
#include "../Commons/Colliders/SphereCollider.h"
#include "../Commons/Colliders/MeshCollider.h"
#include "../Commons/Renderings/Model3D/Model3D.h"
#include "../Commons/Renderings/Model3D/IModel3DRenderer.h"
#include "../Commons/Renderings/Image/Image.h"
#include "../Commons/Renderings/Image/IImageRenderer.h"
#include "../Commons/Renderings/Text/Text.h"
#include "../Commons/Renderings/Text/ITextRenderer.h"
#include "../Commons/Renderings/Particle/ParticleManager.h"
#include "../Commons/Renderings/CameraScreen.h"
#include "../Commons/Renderings/Canvas.h"
#include "../Commons/Systems/IResources.h"

#define REGISTER(T, U) \
componentManager->RegisterCreate<T>([](const ComponentDesc& desc) { return std::make_unique<T>(desc); }); \
gameObjectManager->Register<T>(U)
#define REGISTER_GAMEOBJECT_MANAGER_ONLY(T, U) \
gameObjectManager->Register<T>(U)

// コンポーネント登録
void ComponentsRegisterer::RegisterComponents
(
	Ref<ComponentManager>              componentManager,
	Ref<GameObjectManager>             gameObjectManager,
	Ref<Renderings::IModel3DRenderer>  iModelRenderer,
	Ref<Renderings::IImageRenderer>    iImageRenderer,
	Ref<Renderings::ITextRenderer>     iTextRenderer,
	Ref<Renderings::IColliderRenderer> iColliderRenderer,
	Ref<Systems::IResources>           iResources
)
{
	// 長方形の当たり判定
	componentManager->RegisterCreate<Colliders::BoxCollider>([iColliderRenderer](const ComponentDesc& desc)
		{
			return std::make_unique<Colliders::BoxCollider>(desc, iColliderRenderer);
		});
	// 球の当たり判定
	componentManager->RegisterCreate<Colliders::SphereCollider>([iColliderRenderer](const ComponentDesc& desc)
		{
			return std::make_unique<Colliders::SphereCollider>(desc, &iColliderRenderer.Get());
		});
	// メッシュの当たり判定
	componentManager->RegisterCreate<Colliders::MeshCollider>([iColliderRenderer, iResources](const ComponentDesc& desc)
		{
			return std::make_unique<Colliders::MeshCollider>(desc, &iColliderRenderer.Get(), &iResources.Get());
		});
	// 3Dモデル
	componentManager->RegisterCreate<Renderings::Model3D>([iModelRenderer, iResources](const ComponentDesc& desc)
		{
			return std::make_unique<Renderings::Model3D>(desc, &iModelRenderer.Get(), &iResources.Get());
		});
	// 画像
	componentManager->RegisterCreate<Renderings::Image>([iImageRenderer, iResources](const ComponentDesc& desc)
		{
			return std::make_unique<Renderings::Image>(desc, &iImageRenderer.Get(), &iResources.Get());
		});
	// テキスト
	componentManager->RegisterCreate<Renderings::Text>([iTextRenderer](const ComponentDesc& desc)
		{
			return std::make_unique<Renderings::Text>(desc, &iTextRenderer.Get());
		});
	// パーティクル管理
	componentManager->RegisterCreate<Renderings::ParticleManager>([iModelRenderer, iResources](const ComponentDesc& desc)
		{
			return std::make_unique<Renderings::ParticleManager>(desc, &iModelRenderer.Get(), &iResources.Get());
		});
	// カメラ
	componentManager->RegisterCreate<Renderings::ICameraScreen>([](const ComponentDesc& desc)
		{
			return std::make_unique<Renderings::CameraScreen<Camera::QuaternionCamera>>(desc);
		});
	// アクション管理
	componentManager->RegisterCreate<ActionManager>([iResources](const ComponentDesc& desc)
		{
			return std::make_unique<ActionManager>(desc, &iResources.Get());
		});

	// トランスフォーム
	REGISTER(Transform, "Transform");
	// 2D用トランスフォーム
	REGISTER(RectTransform, "RectTransform");
	// 当たり判定
	REGISTER_GAMEOBJECT_MANAGER_ONLY(Colliders::BoxCollider, "BoxCollider");
	REGISTER_GAMEOBJECT_MANAGER_ONLY(Colliders::SphereCollider, "SphereCollider");
	REGISTER_GAMEOBJECT_MANAGER_ONLY(Colliders::MeshCollider, "MeshCollider");
	// 3Dモデル
	REGISTER_GAMEOBJECT_MANAGER_ONLY(Renderings::Model3D, "Model3D");
	// 画像
	REGISTER_GAMEOBJECT_MANAGER_ONLY(Renderings::Image, "Image");
	// 文字
	REGISTER_GAMEOBJECT_MANAGER_ONLY(Renderings::Text, "Text");
	// 文字
	REGISTER_GAMEOBJECT_MANAGER_ONLY(Renderings::ParticleManager, "ParticleManager");
	// カメラ画面
	REGISTER(Renderings::CameraScreen<Camera::QuaternionCamera>, "QuaternionCameraScreen");
	REGISTER(Renderings::CameraScreen<Camera::QuaternionTargetCamera>, "QuaternionTargetCameraScreen");
	REGISTER(Renderings::CameraScreen<Camera::EulerCamera>, "EulerCameraScreen");
	REGISTER(Renderings::CameraScreen<Camera::EulerTargetCamera>, "EulerTargetCameraScreen");
	// キャンバス
	REGISTER(Renderings::Canvas, "Canvas");
	// シーン
	REGISTER(SampleScene, "SampleScene");
	REGISTER(TitleScene, "TitleScene");
	REGISTER(GamePlayScene, "GamePlayScene");
	// アクション
	REGISTER(Action, "Action");
	REGISTER(ActionOpenMessageWindow, "ActionOpenMessageWindow");
	REGISTER(ActionCloseMessageWindow, "ActionCloseMessageWindow");
	REGISTER(ActionSetMessageWindowText, "ActionSetMessageWindowText");
	REGISTER_GAMEOBJECT_MANAGER_ONLY(ActionManager, "ActionManager");
	// デバッグ用カメラ
	REGISTER(DebugCamera, "DebugCamera");
	// プレイヤー
	REGISTER(Player, "Player");
	REGISTER(PlayerModel, "PlayerModel");
	REGISTER(PlayerWire, "PlayerWire");
	REGISTER(PlayerCamera, "PlayerCamera");
	// NPC管理
	REGISTER(NPC, "NPC");
	REGISTER(NPCManager, "NPCManager");
	// 選択メニュー
	REGISTER(SelectMenu, "SelectMenu");
	// カーソル
	REGISTER(InteractCursor, "InteractCursor");
	// Z: はなす
	REGISTER(Z2Talk, "Z2Talk");
	// 上下の黒帯
	REGISTER(BlackBelt, "BlackBelt");
	// メッセージウィンドウ
	REGISTER(MessageWindow, "MessageWindow");
	// ターゲット照準
	REGISTER(TargetPoint, "TargetPoint");
}
