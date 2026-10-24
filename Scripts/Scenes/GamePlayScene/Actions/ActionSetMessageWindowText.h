/*
 * FileName:     ActionSetMessageWindowText.h
 * Author:       Takao Hayata
 * Last Updated: 2026/09/10
 *
 * メッセージウィンドウテキスト設定アクション
 */

#pragma once

#include "Action.h"

class MessageWindow;

// メッセージウィンドウテキスト設定アクション
class ActionSetMessageWindowText : public Action
{

public:


	/* メンバ関数 */

	// コンストラクタ
	ActionSetMessageWindowText(const ComponentDesc& desc);

	// 初期化処理
	void Initalize(const nlohmann::ordered_json& json, Ref<IGameObjectFinder> iGameObjectFinderRef) override;

	// パラメータを設定
	void SetParams(const nlohmann::ordered_json& json, Ref<IGameObjectFinder> iGameObjectFinderRef) override;

	// 開始処理
	void Enter() override;
	// 更新処理
	void Update(float elapsedTime, const IGameInput& iGameInput) override;


private:


	/* メンバ変数 */

	// 文字列
	std::wstring m_text;
	// 話者
	std::wstring m_character;

	// メッセージウィンドウの参照
	Ref<MessageWindow> m_refMessageWindow;

};