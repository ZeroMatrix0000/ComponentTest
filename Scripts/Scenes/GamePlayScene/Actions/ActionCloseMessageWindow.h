/*
 * FileName:     ActionCloseMessageWindow.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/21
 *
 * メッセージウィンドウ閉アクション
 */

#pragma once

#include "Action.h"

class MessageWindow;

// メッセージウィンドウ閉アクション
class ActionCloseMessageWindow : public Action
{

public:


	/* メンバ関数 */

	// コンストラクタ
	ActionCloseMessageWindow(const ComponentDesc& desc);

	// 初期化処理
	void Initalize(const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder) override;

	// パラメータを設定
	void SetParams(const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder) override;

	// 開始処理
	void Enter() override;
	// 更新処理
	void Update(float elapsedTime, const IGameInput& iGameInput) override;


private:


	/* メンバ変数 */

	// 開き終わるまで待つか
	bool m_isWait;

	// メッセージウィンドウの参照
	Ref<MessageWindow> m_refMessageWindow;

};