/*
 * FileName:     Text.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/09/28
 *
 * テキスト
 */

#include "Pch.h"
#include "Text.h"

#include "ITextRenderer.h"
#include "Scripts/Commons/GameObjects/GameObject.h"
#include "Scripts/Commons/GameObjects/IGameObjectFinder.h"
#include "Scripts/Commons/Renderings/Canvas.h"
#include "Scripts/Commons/Systems/JsonSerializer.h"

// コンストラクタ
Renderings::Text::Text(const ComponentDesc& desc, ITextRenderer* pITextRenderer)
	: Component{ desc }
	, m_str{}
	, m_fontName{}
	, m_fontSize{ 24.0f }
	, m_fontColor{ D2D1::ColorF::White }
	, m_textAlignment{}
	, m_paragraphAlignment{}
	, m_outlineWidth{ 0.0f }
	, m_outlineColor{ D2D1::ColorF::Black }
	, m_orderInLayer{}
	, m_pCanvas{}
	, m_pITextRenderer{ pITextRenderer }
{
	// テキスト描画にポインタを追加
	m_pITextRenderer->AddPText(this);
}

// デストラクタ
Renderings::Text::~Text()
{
	// 画像描画からポインタを削除
	m_pITextRenderer->RemovePText(this);
}

// 初期化処理
void Renderings::Text::Initalize(const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder)
{
	std::wstring str{ m_str.GetSourceStr() };

	Systems::JsonSerializer serializer{ pIGameObjectFinder };
	serializer.AddParameter(&str, "Str");
	serializer.AddParameter(&m_fontName, "FontName");
	serializer.AddParameter(&m_fontSize, "FontSize");
	serializer.AddParameter(&m_fontColor, "FontColor");
	serializer.AddParameter(&m_textAlignment, "TextAlignment");
	serializer.AddParameter(&m_paragraphAlignment, "ParagraphAlignment");
	serializer.AddParameter(&m_outlineWidth, "OutlineWidth");
	serializer.AddParameter(&m_outlineColor, "OutlineColor");
	serializer.AddParameter(&m_orderInLayer, "OrderInLayer");
	serializer.AddParameter(&m_pCanvas, "Canvas");
	serializer.Load(json);

	m_str.SetStr(str);
}
