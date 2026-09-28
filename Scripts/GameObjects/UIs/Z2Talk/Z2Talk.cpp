/*
 * FileName:     Z2Talk.h
 * Author:       Takao Hayata
 * Last Updated: 2026/09/28
 *
 * Z: はなす
 */

#include "Pch.h"
#include "Z2Talk.h"

#include "Scripts/Commons/GameObjects/GameObject.h"
#include "Scripts/Commons/GameObjects/IGameObjectFinder.h"
#include "Scripts/Commons/Components/RectTransform.h"
#include "Scripts/Commons/Renderings/ICameraScreen.h"
#include "Scripts/Commons/Renderings/Canvas.h"
#include "Scripts/Commons/Renderings/Text/Text.h"
#include "Scripts/Commons/Systems/JsonSerializer.h"

// コンストラクタ
Z2Talk::Z2Talk(const ComponentDesc& desc)
	: Component{ desc }
	, m_alpha{}
	, m_alphaVelocity{}
	, m_pRectTransform{ GetPOwner()->GetNullReferences<RectTransform>() }
	, m_pText{ GetPOwner()->GetNullReferences<Renderings::Text>() }
{
}

// 初期化処理
void Z2Talk::Initalize(const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder)
{
	Systems::JsonSerializer serializer{ pIGameObjectFinder };
	serializer.AddParameter(&m_alphaVelocity, "AlphaVelocity");
	serializer.Load(json);

	m_alpha.Initialize(0.0f, 0.0f, 1.0f);
	m_pRectTransform = GetPOwner()->GetComponent<RectTransform>();
	m_pText = GetPOwner()->GetComponent<Renderings::Text>();
}

// 更新処理
void Z2Talk::Update(float elapsedTime, bool isFocused)
{
	// 不透明度を設定
	m_alpha += m_alphaVelocity * elapsedTime * (isFocused ? 1.0f : -1.0f);

	Math::Color color = m_pText->GetFontColor();
	color.w = m_alpha;
	m_pText->SetFontColor(color);
}

// 座標を設定
void Z2Talk::SetPosition(const Math::Box& playerBox, const Renderings::ICameraScreen& iCameraScreen)
{
	// 透明なら何もしない
	if (m_alpha == 0.0f)
	{
		return;
	}

	Math::Vector2 screenPosition = Math::Geometry::Projection
	(
		playerBox.position - playerBox.size * Math::Vector3::Up / 2.0f,
		iCameraScreen.GetViewMatrix(),
		iCameraScreen.GetProjectionMatrix(),
		m_pText->GetPCanvas()->GetSize()
	);

	m_pRectTransform->SetPosition(screenPosition);
}
