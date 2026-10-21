/*
 * FileName:     ActionCloseMessageWindow.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/21
 *
 * メッセージウィンドウ閉アクション
 */

#include "Pch.h"
#include "ActionCloseMessageWindow.h"

#include "Scripts/GameObjects/UIs/MessageWindow/MessageWindow.h"
#include "Scripts/Commons/Systems/JsonSerializer.h"

 // コンストラクタ
ActionCloseMessageWindow::ActionCloseMessageWindow(const ComponentDesc& desc)
	: Action{ desc }
	, m_isWait{}
	, m_refMessageWindow{ GetOwner().GetNullReferences<MessageWindow>() }
{
}

// 初期化処理
void ActionCloseMessageWindow::Initalize(const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder)
{
	Systems::JsonSerializer serializer{ pIGameObjectFinder };
	serializer.AddParameter(&m_refMessageWindow, "MessageWindow");
	serializer.Load(json);
}

// パラメータを設定
void ActionCloseMessageWindow::SetParams(const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder)
{
	Systems::JsonSerializer serializer{ pIGameObjectFinder };
	serializer.AddParameter(&m_isWait, "IsWait");
	serializer.Load(json);
}

// 開始処理
void ActionCloseMessageWindow::Enter()
{
	// ダイアログボックスを出現
	m_refMessageWindow->Appear(false);
}

// 更新処理
void ActionCloseMessageWindow::Update(float elapsedTime, const IGameInput& iGameInput)
{
	// 開き終わるまで待たないか、開き終わったなら次のアクションへ移動
	if (!m_isWait || !m_refMessageWindow->IsMoving())
	{
		GoNext();
	}
}
