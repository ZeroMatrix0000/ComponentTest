/*
 * FileName:     RectTransform.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/24
 *
 * 2D用トランスフォーム
 */

#include "Pch.h"
#include "RectTransform.h"

#include "../Systems/JsonSerializer.h"

// コンストラクタ
Components::RectTransform::RectTransform(const ComponentDesc& desc)
	: Component{ desc }
	, m_rect{}
	, m_angle{}
	, m_pivot{ Utility::AlignmentPoint::MiddleCenter }
	, m_anchor{ Utility::AlignmentPoint::MiddleCenter }
{
}

// 初期化処理
void Components::RectTransform::Initalize(const nlohmann::ordered_json& json, Ref<IGameObjectFinder> iGameObjectFinderRef)
{
	Systems::JsonSerializer serializer{ iGameObjectFinderRef };
	serializer.AddParameter(&m_rect.position, "Position");
	serializer.AddParameter(&m_rect.size, "Size");
	serializer.AddParameter(&m_angle, "Angle");
	serializer.AddParameter(&m_pivot, "Pivot");
	serializer.AddParameter(&m_anchor, "Anchor");
	serializer.Load(json);
}
