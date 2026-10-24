/*
 * FileName:     GamePlaySceneInternals.hpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/23
 *
 * ゲームプレイシーンの内部データ
 */

#include "Pch.h"
#include "GamePlaySceneInternals.h"

#include "Scripts/Main/IsDerivedRegister.h"
#include "Scripts/Commons/GameObjects/GameObject.h"

GamePlaySceneInternals::GamePlaySceneInternals
(
	Ref<std::vector<std::unique_ptr<GameObject>>> gameObjectsRef,
	Ref<const GameContext> gameContextRef,
	Ref<GameObject> ownerRef
)
	: gameObjectsRef{ gameObjectsRef }
	, gameContextRef{ gameContextRef }
	, ownerRef{ ownerRef }
	, playerRef{ ownerRef->GetNullComponent<Player>() }
	, playerCameraRef{ ownerRef->GetNullComponent<PlayerCamera>() }
	, nPCManagerRef{ ownerRef->GetNullComponent<NPCManager>() }
	, groundMeshRef{ ownerRef->GetNullComponent<Colliders::MeshCollider>() }
	, groundModelRef{ ownerRef->GetNullComponent<Renderings::Model3D>() }
	, reticleRef{ ownerRef->GetNullComponent<Renderings::Image>() }
	, z2TalkRef{ ownerRef->GetNullComponent<Z2Talk>() }
	, blackBeltRef{ ownerRef->GetNullComponent<BlackBelt>() }
	, messageWindowRef{ ownerRef->GetNullComponent<MessageWindow>() }
	, targetPointRef{ ownerRef->GetNullComponent<TargetPoint>() }
	, iCameraScreenRef{ ownerRef->GetNullComponent<Renderings::ICameraScreen>() }
	, canvasRef{ ownerRef->GetNullComponent<Renderings::Canvas>() }
	, actionManagerRef{ ownerRef->GetNullComponent<ActionManager>() }
{
}
