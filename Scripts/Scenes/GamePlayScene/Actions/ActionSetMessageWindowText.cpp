/*
 * FileName:     ActionSetMessageWindowText.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/23
 *
 * メッセージウィンドウ開アクション
 */

#include "Pch.h"
#include "ActionSetMessageWindowText.h"

#include "Scripts/Main/IsDerivedRegister.h"
#include "Scripts/Main/IGameInput.h"
#include "Scripts/GameObjects/UIs/MessageWindow/MessageWindow.h"
#include "Scripts/Commons/Systems/JsonSerializer.h"

 // コンストラクタ
ActionSetMessageWindowText::ActionSetMessageWindowText(const ComponentDesc& desc)
	: Action{ desc }
	, m_text{}
	, m_character{}
	, m_refMessageWindow{ GetOwner()->GetNullReference<MessageWindow>() }
{
}

// 初期化処理
void ActionSetMessageWindowText::Initalize(const nlohmann::ordered_json& json, Ref<IGameObjectFinder> iGameObjectFinder)
{
	Systems::JsonSerializer serializer{ iGameObjectFinder };
	serializer.AddParameter(&m_refMessageWindow, "MessageWindow");
	serializer.Load(json);
}

// パラメータを設定
void ActionSetMessageWindowText::SetParams(const nlohmann::ordered_json& json, Ref<IGameObjectFinder> iGameObjectFinder)
{
	Systems::JsonSerializer serializer{ iGameObjectFinder };
	serializer.AddParameter(&m_text, "Text");
	serializer.AddParameter(&m_character, "Character");
	serializer.Load(json);
}

// 開始処理
void ActionSetMessageWindowText::Enter()
{
	// テキストを設定
	m_refMessageWindow->SetText(m_text);
	m_refMessageWindow->SetCharacter(m_character);
}

// 更新処理
void ActionSetMessageWindowText::Update(float elapsedTime, const IGameInput& iGameInput)
{
	// テキスト送り入力がされたら
	if (iGameInput.GetTextScroll())
	{
		// 全て表示されていたら次のアクションへ移動、でなければ全て表示
		if (m_refMessageWindow->IsDisplayAllText())
		{
			GoNext();
		}
		else
		{
			m_refMessageWindow->DisplayAllText();
		}
	}
}
