/*
 * FileName:     SelectMenu.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/09/28
 *
 * 選択メニュー
 */

#include "Pch.h"
#include "SelectMenu.h"

#include "Scripts/Commons/Renderings/Image/Image.h"
#include "Scripts/Commons/Renderings/Text/Text.h"
#include "Scripts/Commons/Renderings/Canvas.h"
#include "Scripts/Commons/GameObjects/GameObject.h"
#include "Scripts/Commons/Components/RectTransform.h"
#include "Scripts/Commons/GameObjects/IGameObjectFinder.h"
#include "Scripts/Commons/Systems/JsonSerializer.h"

// コンストラクタ
SelectMenu::SelectMenu(const ComponentDesc& desc)
	: Component{ desc }
	, m_params{}
	, m_selectNumber{}
	, m_cursorDelayY{}
	, m_cursorSwayTimer{}
	, m_pTexts{}
	, m_Processes{}
	, m_fontSizes{}
	, m_pCursorImage{ GetPOwner()->GetNullReferences<Renderings::Image>() }
	, m_pCursorRectTransform{ GetPOwner()->GetNullReferences<RectTransform>() }
{
}

// 初期化処理
void SelectMenu::Initalize(const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder)
{
	if (m_pCursorImage == GetPOwner()->GetNullReferences<Renderings::Image>())
	{
		GameObject* pObj = Instantiate("Prefab_SelectMenuCursor");
		m_pCursorImage = pObj->GetComponent<Renderings::Image>();
		m_pCursorRectTransform = pObj->GetComponent<RectTransform>();
	}

	// 選択番号を初期化
	m_selectNumber = 0;
	// カーソルのY座標のズレ
	m_cursorDelayY.SetMovement(0.0f, 0.0f);

	float cursorSwayTime = m_cursorSwayTimer.GetMax();
	const Renderings::Canvas* pCanvas = m_pCursorImage->GetPCanvas();

	Systems::JsonSerializer serializer{ pIGameObjectFinder };
	serializer.AddParameter(&m_params.width, "Width");
	serializer.AddParameter(&m_params.height, "Height");
	serializer.AddParameter(&m_params.interval, "Interval");
	serializer.AddParameter(&m_params.basePosition, "Position");
	serializer.AddParameter(&m_params.cursorMoveTime, "CursorMoveTime");
	serializer.AddParameter(&m_params.cursorEasingType, "CursorEasingType");
	serializer.AddParameter(&m_params.cursorEasingInOut, "CursorEasingInOut");
	serializer.AddParameter(&cursorSwayTime, "CursorSwayTime");
	serializer.AddParameter(&m_params.cursorSwaySize, "CursorSwaySize");
	serializer.AddParameter(&m_params.fontSizeBegin, "FontSizeBegin");
	serializer.AddParameter(&m_params.fontSizeEnd, "FontSizeEnd");
	serializer.AddParameter(&pCanvas, "Canvas");
	serializer.Load(json);

	m_cursorSwayTimer.Initialize(0.0f, 0.0f, cursorSwayTime);
	m_pCursorImage->SetCanvas(*pCanvas);
}

// 更新処理
void SelectMenu::Update(float elapsedTime)
{
	// 表示座標
	Math::Vector2 position = m_params.basePosition;

	// カーソルのY座標のズレを更新
	m_cursorDelayY.Tick(elapsedTime);
	// カーソルの左右のズレの周期を更新
	m_cursorSwayTimer += elapsedTime;

	// カーソルの左右のズレ
	Math::Vector2 cursorDelay = Math::Vector2
	{
		m_params.cursorSwaySize * Math::Sin(m_cursorSwayTimer.GetRatio() * 2.0f * Math::PI),
		(m_params.height + m_params.interval) * m_cursorDelayY.GetMovement()
	};

	// カーソルの位置を変更
	m_pCursorRectTransform->SetPosition(m_params.basePosition + Math::Vector2{ -m_params.width / 2.0f, 0.0f } + cursorDelay);

	// フォントサイズを変更
	for (size_t i = 0; i < m_pTexts.size(); i++)
	{
		m_fontSizes.at(i).Tick(elapsedTime);
		m_pTexts.at(i)->SetFontSize(m_fontSizes.at(i).GetMovement());
	}
}

// 選択肢を追加
void SelectMenu::AddOption(const std::wstring& str, const std::function<void()>& Process)
{
	GameObject* gameObject = Instantiate("Prefab_SelectMenuText");
	auto* pText = gameObject->AddComponent<Renderings::Text>();
	pText->SetStr(str);
	pText->SetCanvas(*m_pCursorImage->GetPCanvas());
	pText->SetFontSize(m_params.fontSizeBegin);
	auto* pRectTransform = gameObject->AddComponent<RectTransform>();
	pRectTransform->SetPosition(m_params.basePosition + Math::Vector2{ 0.0f, m_params.height + m_params.interval } * static_cast<float>(m_Processes.size()));
	pRectTransform->SetSize(Math::Vector2{ m_params.width, m_params.height });

	m_pTexts.push_back(pText);
	m_Processes.push_back(Process);
	m_fontSizes.push_back(Easing::Value<float>{});
	m_fontSizes.back().SetMovement(static_cast<int>(m_fontSizes.size()) == m_selectNumber + 1 ? m_params.fontSizeEnd : m_params.fontSizeBegin, 0.0f);
}

// 上の項目を選択
void SelectMenu::SelectUp()
{
	if (m_Processes.size() == 0)
	{
		return;
	}
	m_fontSizes.at(m_selectNumber).SetMovement(m_params.fontSizeBegin, m_params.cursorMoveTime, m_params.cursorEasingType, m_params.cursorEasingInOut);
	m_selectNumber = (m_selectNumber + m_Processes.size() - 1) % static_cast<int>(m_Processes.size());
	m_fontSizes.at(m_selectNumber).SetMovement(m_params.fontSizeEnd, m_params.cursorMoveTime, m_params.cursorEasingType, m_params.cursorEasingInOut);
	m_cursorDelayY.SetMovement(static_cast<float>(m_selectNumber), m_params.cursorMoveTime, m_params.cursorEasingType, m_params.cursorEasingInOut);
}

// 下の項目を選択
void SelectMenu::SelectDown()
{
	if (m_Processes.size() == 0)
	{
		return;
	}
	m_fontSizes.at(m_selectNumber).SetMovement(m_params.fontSizeBegin, m_params.cursorMoveTime, m_params.cursorEasingType, m_params.cursorEasingInOut);
	m_selectNumber = (m_selectNumber + 1) % static_cast<int>(m_Processes.size());
	m_fontSizes.at(m_selectNumber).SetMovement(m_params.fontSizeEnd, m_params.cursorMoveTime, m_params.cursorEasingType, m_params.cursorEasingInOut);
	m_cursorDelayY.SetMovement(static_cast<float>(m_selectNumber), m_params.cursorMoveTime, m_params.cursorEasingType, m_params.cursorEasingInOut);
}

// 実行
void SelectMenu::Execute()
{
	if (m_Processes.size() == 0)
	{
		return;
	}
	m_Processes.at(m_selectNumber)();
}
