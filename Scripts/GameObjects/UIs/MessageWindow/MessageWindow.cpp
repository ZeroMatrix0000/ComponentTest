/*
 * FileName:     MessageWindow.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/21
 *
 * メッセージウィンドウ
 */

#include "Pch.h"
#include "MessageWindow.h"

#include "Scripts/Commons/Components/RectTransform.h"
#include "Scripts/Commons/Renderings/Canvas.h"
#include "Scripts/Commons/Renderings/Image/Image.h"
#include "Scripts/Commons/Renderings/Text/Text.h"
#include "Scripts/Commons/Systems/JsonSerializer.h"

// コンストラクタ
MessageWindow::MessageWindow(const ComponentDesc& desc)
	: Component{ desc }
	, m_moveTime{}
	, m_textVelocity{}
	, m_backgroundSize{}
	, m_characterSize{}
	, m_textSize{}
	, m_characterFontSize{}
	, m_textFontSize{}
	, m_moveRate{}
	, m_text{}
	, m_textCount{}
	, m_refBackgroundTransform{ GetOwner()->GetNullReference<RectTransform>() }
	, m_refBackground{ GetOwner()->GetNullReference<Renderings::Image>() }
	, m_refTextTransform{ GetOwner()->GetNullReference<RectTransform>() }
	, m_refText{ GetOwner()->GetNullReference<Renderings::Text>() }
	, m_refCharacterTransform{ GetOwner()->GetNullReference<RectTransform>() }
	, m_refCharacter{ GetOwner()->GetNullReference<Renderings::Text>() }
{
}

// 初期化処理
void MessageWindow::Initalize(const nlohmann::ordered_json& json, Ref<IGameObjectFinder> iGameObjectFinder)
{
	const Renderings::Canvas* pCanvas = nullptr;

	Systems::JsonSerializer serializer{ iGameObjectFinder };
	serializer.AddParameter(&m_moveTime, "MoveTime");
	serializer.AddParameter(&m_textVelocity, "TextVelocity");
	serializer.AddParameter(&pCanvas, "Canvas");
	serializer.Load(json);

	// メッセージウィンドウを生成
	if (m_refBackgroundTransform == GetOwner()->GetNullReference<RectTransform>())
	{
		Ref<GameObject> refObj = Instantiate("Prefab_MessageWindowBackGround");
		m_refBackgroundTransform = refObj->GetComponent<RectTransform>();
		m_refBackground = refObj->GetComponent<Renderings::Image>();
		refObj = Instantiate("Prefab_MessageWindowCharacter");
		m_refCharacterTransform = refObj->GetComponent<RectTransform>();
		m_refCharacter = refObj->GetComponent<Renderings::Text>();
		refObj = Instantiate("Prefab_MessageWindowText");
		m_refTextTransform = refObj->GetComponent<RectTransform>();
		m_refText = refObj->GetComponent<Renderings::Text>();
	}

	// キャンバスを設定
	if (pCanvas)
	{
		m_refBackground->SetCanvas(*pCanvas);
		m_refCharacter->SetCanvas(*pCanvas);
		m_refText->SetCanvas(*pCanvas);
	}

	// 移動率を初期化
	m_moveRate.SetMovement(0.0f, 0.0f);

	// サイズを取得
	m_backgroundSize = m_refBackgroundTransform->GetRect().size;
	m_characterSize = m_refCharacterTransform->GetRect().size;
	m_textSize = m_refTextTransform->GetRect().size;
	m_characterFontSize = m_refCharacter->GetFontSize();
	m_textFontSize = m_refText->GetFontSize();

	// 非表示
	m_refBackgroundTransform->SetSize(Math::Vector2::Zero);
	m_refCharacterTransform->SetSize(Math::Vector2::Zero);
	m_refTextTransform->SetSize(Math::Vector2::Zero);
	m_refCharacter->SetFontSize(0.0f);
	m_refText->SetFontSize(0.0f);

}

// 更新処理
void MessageWindow::Update(float elapsedTime)
{
	// 移動率を更新
	m_moveRate.Tick(elapsedTime);
	// 移動率
	float moveRate = m_moveRate.GetMovement();

	// 透明度を変更
	Math::Color color = m_refBackground->GetColor();
	color.w = moveRate;
	m_refBackground->SetColor(color);
	color = m_refCharacter->GetFontColor();
	color.w = moveRate;
	m_refCharacter->SetFontColor(color);
	color = m_refText->GetFontColor();
	color.w = moveRate;
	m_refText->SetFontColor(color);

	// 古い文字数
	size_t oldTextCount = Math::RoundInt(m_textCount);
	// 文字数を計算
	m_textCount += m_textVelocity * elapsedTime;
	// 新しく表示する文字数
	size_t newCharCount = Math::RoundInt(m_textCount) - oldTextCount;

	// 検索番号
	size_t index = oldTextCount;

	// <> の間か
	bool isAngleBracket = false;
	// 文字列に変更があったか
	bool isChanged = false;

	while (newCharCount != 0 && !m_textCount.IsMax())
	{
		// 文字
		wchar_t c = m_text.at(index);

		if (isAngleBracket)
		{
			m_textCount += 1.0f;
			if (c == L'>')
			{
				isAngleBracket = false;
			}
		}
		else
		{
			if (c == L'<')
			{
				isAngleBracket = true;
				m_textCount += 1.0f;
			}
			else
			{
				isChanged = true;
				newCharCount--;
			}
		}

		index++;
	}

	// 移動率が 0 なら以降何もしない
	if (moveRate == 0.0f)
	{
		return;
	}

	// サイズを変更
	m_refBackgroundTransform->SetSize(Math::Vector2::Lerp(Math::Vector2::Zero, m_backgroundSize, moveRate));
	m_refCharacterTransform->SetSize(Math::Vector2::Lerp(Math::Vector2::Zero, m_characterSize, moveRate));
	m_refTextTransform->SetSize(Math::Vector2::Lerp(Math::Vector2::Zero, m_textSize, moveRate));

	// フォントサイズを変更
	m_refCharacter->SetFontSize(m_characterFontSize * moveRate);
	m_refText->SetFontSize(m_textFontSize * moveRate);

	// 文字列を変更
	if (isChanged)
	{
		std::wstring rubyStr = m_text.substr(Math::RoundInt(m_textCount));
		size_t rubyIndex = rubyStr.find(L"</ruby>");
		if (rubyIndex != std::wstring::npos)
		{
			rubyStr = rubyStr.substr(0, rubyIndex);
		}
		Renderings::DecorationString::DeleteTag(&rubyStr);

		std::wstring str = m_text.substr(0, Math::RoundInt(m_textCount));
		str.append(L"<color=#00000000>");
		str.append(rubyStr);

		m_refText->SetStr(str);
	}
}

// 出現
void MessageWindow::Appear(bool isAppear)
{
	m_moveRate.SetMovement
	(
		isAppear ? 0.0f : 1.0f,
		isAppear ? 1.0f : 0.0f,
		m_moveTime,
		Easing::Type::Back,
		isAppear ? Easing::InOut::Out : Easing::InOut::In
	);
}

// テキストを設定
void MessageWindow::SetText(const std::wstring& text)
{
	m_text = text;
	m_textCount.Initialize(0.0f, 0.0f, static_cast<float>(text.size()));
	m_refText->SetStr(L"");
}

// テキストを全て表示
void MessageWindow::DisplayAllText()
{
	m_textCount = static_cast<float>(m_text.size());
	m_refText->SetStr(m_text);
}

// 話者を設定
void MessageWindow::SetCharacter(const std::wstring& text)
{
	// 空文字でなければ
	if (!text.empty())
	{
		m_refCharacter->SetStr(text);
	}
}
