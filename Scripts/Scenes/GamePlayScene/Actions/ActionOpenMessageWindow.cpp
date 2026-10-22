/*
 * FileName:     ActionOpenMessageWindow.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/21
 *
 * メッセージウィンドウ開アクション
 */

#include "Pch.h"
#include "ActionOpenMessageWindow.h"

#include "Scripts/GameObjects/UIs/MessageWindow/MessageWindow.h"
#include "Scripts/Commons/Systems/JsonSerializer.h"

// コンストラクタ
ActionOpenMessageWindow::ActionOpenMessageWindow(const ComponentDesc& desc)
	: Action{ desc }
	, m_isWait{}
	, m_refMessageWindow{ GetOwner()->GetNullReference<MessageWindow>() }
{
}

// 初期化処理
void ActionOpenMessageWindow::Initalize(const nlohmann::ordered_json& json, Ref<IGameObjectFinder> iGameObjectFinder)
{
	Systems::JsonSerializer serializer{ iGameObjectFinder };
	serializer.AddParameter(&m_refMessageWindow, "MessageWindow");
	serializer.Load(json);
}

// パラメータを設定
void ActionOpenMessageWindow::SetParams(const nlohmann::ordered_json& json, Ref<IGameObjectFinder> iGameObjectFinder)
{
	Systems::JsonSerializer serializer{ iGameObjectFinder };
	serializer.AddParameter(&m_isWait, "IsWait");
	serializer.Load(json);
}

// 開始処理
void ActionOpenMessageWindow::Enter()
{
	// ダイアログボックスを出現
	m_refMessageWindow->Appear(true);
	// テキストを初期化
	m_refMessageWindow->SetText();
	m_refMessageWindow->SetCharacter();
}

// 更新処理
void ActionOpenMessageWindow::Update(float elapsedTime, const IGameInput& iGameInput)
{
	// 開き終わるまで待たないか、開き終わったなら次のアクションへ移動
	if (!m_isWait || !m_refMessageWindow->IsMoving())
	{
		GoNext();
	}
}
