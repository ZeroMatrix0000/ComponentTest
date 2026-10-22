/*
 * FileName:     Canvas.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/09/11
 *
 * カメラ画面
 */

#include "Pch.h"
#include "Canvas.h"

#include "../Systems/JsonSerializer.h"

// コンストラクタ
Renderings::Canvas::Canvas(const ComponentDesc& desc)
	: Component{ desc }
	, m_fixedSize{}
	, m_size{}
	, m_ratio{}
{
}

// 初期化処理
void Renderings::Canvas::Initalize(const nlohmann::ordered_json& json, Ref<IGameObjectFinder> iGameObjectFinder)
{
	// 出力サイズ
	Math::Vector2 outputSize{};

	Systems::JsonSerializer serializer{ iGameObjectFinder };
	serializer.AddParameter(&m_fixedSize, "FixedSize");
	serializer.AddParameter(&outputSize, "Size");
	serializer.Load(json);

	if(outputSize != Math::Vector2::Zero)
	{
		SetSize(outputSize);
	}
}

// 初期化処理
void Renderings::Canvas::Initialize(FixedSize fixedSize, const Math::Vector2& outputSize)
{
	m_fixedSize = fixedSize;
	SetSize(outputSize);
}

// サイズを設定
void Renderings::Canvas::SetSize(const Math::Vector2& outputSize)
{
	switch (m_fixedSize)
	{
	case Renderings::Canvas::FixedSize::None:
		m_ratio = 1.0f;
		break;
	case Renderings::Canvas::FixedSize::Horizontal:
		m_ratio = outputSize.x / DEFAULT_SIZE.x;
		break;
	case Renderings::Canvas::FixedSize::Vertical:
		m_ratio = outputSize.y / DEFAULT_SIZE.y;
		break;
	default:
		break;
	}

	m_size = outputSize / m_ratio;
}
