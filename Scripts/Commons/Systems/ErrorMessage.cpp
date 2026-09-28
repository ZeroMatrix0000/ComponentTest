/*
 * FileName:     ErrorMessage.h
 * Author:       Takao Hayata
 * Last Updated: 2026/09/28
 *
 * エラーメッセージ
 */

#include "Pch.h"
#include "ErrorMessage.h"

#include "../GameObjects/GameObject.h"
#include "../Components/RectTransform.h"
#include "../Renderings/Text/Text.h"
#include "../Renderings/Canvas.h"
#include "../GameObjects/IGameObjectInstantiator.h"

// コンストラクタ
Systems::ErrorMessage::ErrorMessage()
	: IErrorMessage{}
	, m_messages{}
	, m_textComponent{}
	, m_displayTime{}
	, m_displayMax{}
	, m_isActive{}
	, m_isChanged{}
{
}

// 初期化処理
void Systems::ErrorMessage::Initialize(float displayTime, size_t displayMax, bool isActive)
{
	m_displayTime = displayTime;
	m_displayMax = displayMax;

	m_isActive = isActive;
}

// 更新処理
void Systems::ErrorMessage::Update(float elapsedTime)
{

	for (int i = 0; i < m_messages.size(); i++)
	{
		m_messages.at(i).timer += elapsedTime;
		// 表示時間を超えたらメッセージを消す
		if (m_messages.at(i).timer.IsMax())
		{
			auto it = m_messages.begin() + i;
			m_isChanged = true;
			m_messages.erase(it);
			i--;
		}
	}

	// 変更があれば表示を適用
	if (m_isChanged)
	{
		m_isChanged = false;
		ApplyDisplayText();
	}
}

// オブジェクト生成
void Systems::ErrorMessage::CreateObjects(IGameObjectInstantiator* pIGameObjectInstantiator)
{
	// テキストの設定
	m_textComponent = pIGameObjectInstantiator->Instantiate("Prefab_ErrorMessage", true)->GetComponent<Renderings::Text>();
	// キャンバスの設定
	auto* pCanvas = pIGameObjectInstantiator->Instantiate("Prefab_Canvas", true)->GetComponent<Renderings::Canvas>();
	pCanvas->Initialize(Renderings::Canvas::FixedSize::None, Math::Vector2{ 1600.0f, 900.0f });

	// キャンバスに画像を映す
	m_textComponent->SetCanvas(*pCanvas);
}

// メッセージを追加
void Systems::ErrorMessage::AddMessage(const std::wstring& text)
{
	// アクティブでないなら何もしない
	if (!m_isActive)
	{
		return;
	}

	m_isChanged = true;
	m_messages.push_back(MessageData{ text, Limited::Create(0.0f, 0.0f, m_displayTime) });

	// 最大数を超えるなら先頭を削除
	if (m_messages.size() > m_displayMax)
	{
		m_messages.erase(m_messages.begin());
	}
}

// 表示を適用
void Systems::ErrorMessage::ApplyDisplayText()
{
	std::wstring str = L"";

	// 各メッセージのテキストを追加
	for (const auto& message : m_messages)
	{
		str += message.text + L"\n";
	}

	m_textComponent->SetStr(str);
}
