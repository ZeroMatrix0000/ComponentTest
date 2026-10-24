/*
 * FileName:     DebugCamera.h
 * Author:       Takao Hayata
 * Last Updated: 2026/07/21
 *
 * デバッグ用カメラ
 */

#pragma once

#include "DebugCameraInternals.h"
#include "Scripts/Commons/Components/Component.h"
#include "Scripts/Commons/Systems/State.h"

// デバッグ用カメラ
class DebugCamera : public Component
{

public:


	/* メンバ関数 */

	// コンストラクタ
	DebugCamera(const ComponentDesc& desc);

	// 初期化処理
	void Initalize(const nlohmann::ordered_json& json, Ref<IGameObjectFinder> iGameObjectFinderRef) override;

	// 更新処理
	void Update(float elapsedTime);

	// カメラ画面を設定
	void SetPCameraScreen();
	// 入力を設定
	void SetInput(const Math::Vector2Int& mouseMovement, bool mouseButtonRight, bool mouseButtonMiddle, int mouseWheelDelta);


private:


	/* メンバ関数 */

	// 状態を変更
	void SetState(std::unique_ptr<Systems::State<DebugCameraInternals>> state);


	/* メンバ変数 */

	// 内部データ
	DebugCameraInternals m_internals;

	// 現在の状態
	std::unique_ptr<Systems::State<DebugCameraInternals>> m_currentState;

};
