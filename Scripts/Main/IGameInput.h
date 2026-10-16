/*
 * FileName:     IGameInput.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/16
 *
 * ゲーム入力のインタフェース
 */

#pragma once

#include "../Commons/Systems/OnlyOne.h"

class IGameInput : public Systems::OnlyOne
{

public:


	/* メンバ関数 */

	// コンストラクタ
	IGameInput()
		: OnlyOne{ typeid(IGameInput) }
	{
	}

	// プレイヤーの移動入力を取得
	virtual Math::Vector3 GetPlayerMove()         const = 0;
	// プレイヤーのカメラの回転入力を取得
	virtual Math::Vector2 GetPlayerCameraRotate() const = 0;
	// プレイヤーのダッシュ入力を取得
	virtual bool          GetPlayerDash()         const = 0;
	// プレイヤーのジャンプ入力を取得
	virtual bool          GetPlayerJump()         const = 0;
	// プレイヤーの糸伸ばし入力を取得
	virtual bool          GetPlayerExpandThread() const = 0;

	// フィールド上のオブジェクトのインタラクト入力を取得
	virtual bool GetFieldInteract() const = 0;

	// テキスト送り入力
	virtual bool GetTextScroll() const = 0;

};
