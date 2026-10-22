/*
 * FileName:     DebugCamera.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/21
 *
 * デバッグ用カメラ
 */

#include "Pch.h"
#include "DebugCamera.h"

#include "State/DebugCameraStateIdle.h"
#include "Scripts/Commons/GameObjects/GameObject.h"

// コンストラクタ
DebugCamera::DebugCamera(const ComponentDesc& desc)
	: Component{ desc }
	, m_internals{}
	, m_currentState{}
{
	// 現在の状態を初期化
	SetState(std::make_unique<DebugCameraStateIdle>());
}

// 初期化処理
void DebugCamera::Initalize(const nlohmann::ordered_json& json, Ref<IGameObjectFinder> iGameObjectFinder)
{
	SetPCameraScreen();
}

// 更新処理
void DebugCamera::Update(float elapsedTime)
{
	// 次の状態に遷移
	std::unique_ptr<Systems::State<DebugCameraInternals>> nextState{ m_currentState->MoveNextState() };
	if (nextState)
	{
		SetState(std::move(nextState));
	}

	// 状態ごとの処理
	m_currentState->Update(&m_internals, elapsedTime);
}

// カメラ画面を設定
void DebugCamera::SetPCameraScreen()
{
	m_internals.pCameraScreen = &GetOwner()->GetComponent<Renderings::CameraScreen<Camera::EulerTargetCamera>>().Get();
}

// 入力を設定
void DebugCamera::SetInput(const Math::Vector2Int& mouseMovement, bool mouseButtonRight, bool mouseButtonMiddle, int mouseWheelDelta)
{
	m_internals.mouseMovement     = mouseMovement;
	m_internals.mouseButtonRight  = mouseButtonRight;
	m_internals.mouseButtonMiddle = mouseButtonMiddle;
	m_internals.mouseWheelDelta   = mouseWheelDelta;
}

// 状態を変更
void DebugCamera::SetState(std::unique_ptr<Systems::State<DebugCameraInternals>> state)
{
	if (m_currentState != nullptr)
	{
		m_currentState->Exit(&m_internals);
	}
	m_currentState = std::move(state);
	m_currentState->Enter(&m_internals);
}
