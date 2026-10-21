/*
 * FileName:     DebugCameraInternals.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/21
 *
 * デバッグ用カメラの内部データ
 */

#pragma once

#include "Scripts/Commons/Renderings/CameraScreen.h"

// デバッグ用カメラの内部データ
struct DebugCameraInternals
{
	// マウスの移動量
	Math::Vector2Int mouseMovement{};
	// 右クリックの状態
	bool mouseButtonRight{};
	// 中クリックの状態
	bool mouseButtonMiddle{};
	// マウスホイールの回転量
	int mouseWheelDelta{};

	// カメラ画面の参照
	Renderings::CameraScreen<Camera::EulerTargetCamera>* pCameraScreen{};
};
