/*
 * FileName:     InteractCursor.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/09/28
 *
 * 話しかける・調べる際のカーソル
 */

#include "Pch.h"
#include "InteractCursor.h"

#include "Scripts/Commons/GameObjects/GameObject.h"
#include "Scripts/Commons/GameObjects/IGameObjectFinder.h"
#include "Scripts/Commons/Components/RectTransform.h"
#include "Scripts/Commons/Renderings/Text.h"
#include "Scripts/Commons/Renderings/Image.h"
#include "Scripts/Commons/Systems/JsonSerializer.h"

// コンストラクタ
InteractCursor::InteractCursor(const ComponentDesc& desc)
	: Component{ desc }
	, m_alphaVelocity{}
	, m_alpha{}
	, m_pTextTransform{ GetPOwner()->GetNullReferences<RectTransform>() }
	, m_pText{ GetPOwner()->GetNullReferences<Renderings::Text>() }
	, m_pImageTransform{ GetPOwner()->GetNullReferences<RectTransform>() }
	, m_pImage{ GetPOwner()->GetNullReferences<Renderings::Image>() }
{
}

// 初期化処理
void InteractCursor::Initalize(const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder)
{
	Systems::JsonSerializer serializer{ pIGameObjectFinder };
	serializer.AddParameter(&m_alphaVelocity, "AlphaVelocity");
	serializer.Load(json);

	m_alpha.Initialize(0.0f, 0.0f, 1.0f);

	if (m_pTextTransform == GetPOwner()->GetNullReferences<RectTransform>())
	{
		GameObject* pObj = Instantiate("Prefab_InteractCursorText");
		m_pTextTransform = pObj->GetComponent<RectTransform>();
		m_pText = pObj->GetComponent<Renderings::Text>();
		pObj = Instantiate("Prefab_InteractCursorImage");
		m_pImageTransform = pObj->GetComponent<RectTransform>();
		m_pImage = pObj->GetComponent<Renderings::Image>();
	}
}

// 更新処理
void InteractCursor::Update(float elapsedTime, bool isFocused)
{
	// 不透明度を設定
	m_alpha += m_alphaVelocity * elapsedTime * (isFocused ? 1.0f : -1.0f);

	Math::Color textColor = m_pText->GetFontColor();
	textColor.w = m_alpha;
	m_pText->SetFontColor(textColor);

	Math::Color imageColor = m_pImage->GetColor();
	imageColor.w = m_alpha;
	m_pImage->SetColor(imageColor);
}

// 名前を設定
void InteractCursor::SetName(const std::wstring& name)
{
	m_pText->SetStr(name);
}

// キャンバスを設定
void InteractCursor::SetCanvas(const Renderings::Canvas& canvas)
{
	m_pText->SetCanvas(canvas);
	m_pImage->SetCanvas(canvas);
}

// 座標を設定
void InteractCursor::SetPosition(const Math::Vector2& position)
{
	// 透明なら何もしない
	if (m_alpha == 0.0f)
	{
		return;
	}

	m_pTextTransform->SetPosition(position);
	m_pImageTransform->SetPosition(position);
}

// キャンバスを取得
const Renderings::Canvas& InteractCursor::GetCanvas() const
{
	return *m_pText->GetPCanvas();
}
