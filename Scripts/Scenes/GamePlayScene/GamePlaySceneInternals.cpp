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
	, playerRef{ ownerRef->GetNullReference<Player>() }
	, playerCameraRef{ ownerRef->GetNullReference<PlayerCamera>() }
	, nPCManagerRef{ ownerRef->GetNullReference<NPCManager>() }
	, groundMeshRef{ ownerRef->GetNullReference<Colliders::MeshCollider>() }
	, groundModelRef{ ownerRef->GetNullReference<Renderings::Model3D>() }
	, reticleRef{ ownerRef->GetNullReference<Renderings::Image>() }
	, z2TalkRef{ ownerRef->GetNullReference<Z2Talk>() }
	, blackBeltRef{ ownerRef->GetNullReference<BlackBelt>() }
	, messageWindowRef{ ownerRef->GetNullReference<MessageWindow>() }
	, targetPointRef{ ownerRef->GetNullReference<TargetPoint>() }
	, iCameraScreenRef{ ownerRef->GetNullReference<Renderings::ICameraScreen>() }
	, canvasRef{ ownerRef->GetNullReference<Renderings::Canvas>() }
	, actionManagerRef{ ownerRef->GetNullReference<ActionManager>() }
{
}
