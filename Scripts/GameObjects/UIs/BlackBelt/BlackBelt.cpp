/*
 * FileName:     BlackBelt.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/09/28
 *
 * 上下の黒帯
 */

#include "Pch.h"
#include "BlackBelt.h"

#include "Scripts/Commons/Components/RectTransform.h"
#include "Scripts/Commons/Renderings/Canvas.h"
#include "Scripts/Commons/Renderings/Image/Image.h"
#include "Scripts/Commons/Systems/JsonSerializer.h"

// コンストラクタ
BlackBelt::BlackBelt(const ComponentDesc& desc)
	: Component{ desc }
	, m_moveTime{}
	, m_height{}
	, m_moveRate{}
	, m_pTopImageTransform{ GetPOwner()->GetNullReferences<RectTransform>() }
	, m_pTopImage{ GetPOwner()->GetNullReferences<Renderings::Image>() }
	, m_pBottomImageTransform{ GetPOwner()->GetNullReferences<RectTransform>() }
	, m_pBottomImage{ GetPOwner()->GetNullReferences<Renderings::Image>() }
{
}

// 初期化処理
void BlackBelt::Initalize(const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder)
{
	m_moveRate.SetMovement(0.0f, 0.0f);

	const Renderings::Canvas* pCanvas = nullptr;

	Systems::JsonSerializer serializer{ pIGameObjectFinder };
	serializer.AddParameter(&m_moveTime, "MoveTime");
	serializer.AddParameter(&pCanvas, "Canvas");
	serializer.Load(json);

	// 画像を生成
	if (m_pTopImageTransform == GetPOwner()->GetNullReferences<RectTransform>())
	{
		GameObject* pObj = Instantiate("Prefab_BlackBeltImage");
		m_pTopImageTransform = pObj->GetComponent<RectTransform>();
		m_pTopImage = pObj->GetComponent<Renderings::Image>();
		m_pTopImageTransform->SetAnchor(Utility::AlignmentPoint::TopCenter);
		pObj = Instantiate("Prefab_BlackBeltImage");
		m_pBottomImageTransform = pObj->GetComponent<RectTransform>();
		m_pBottomImage = pObj->GetComponent<Renderings::Image>();
		m_pBottomImageTransform->SetAnchor(Utility::AlignmentPoint::BottomCenter);
		
		// 高さを取得
		m_height = m_pTopImageTransform->GetRect().size.y;
	}

	// キャンバスを設定
	if (pCanvas)
	{
		m_pTopImage->SetCanvas(*pCanvas);
		m_pBottomImage->SetCanvas(*pCanvas);
	}
}

// 更新処理
void BlackBelt::Update(float elapsedTime)
{
	// 移動率を更新
	m_moveRate.Tick(elapsedTime);
	// 移動率
	float moveRate = m_moveRate.GetMovement();

	// 透明度を変更
	Math::Color color = m_pTopImage->GetColor();
	color.w = moveRate;
	m_pTopImage->SetColor(color);
	m_pBottomImage->SetColor(color);

	// 移動率が 0 なら以降何もしない
	if (m_moveRate.GetMovement() == 0.0f)
	{
		return;
	}

	// サイズを変更
	Math::Vector2 size = m_pTopImageTransform->GetRect().size;
	size.y = m_height * moveRate;
	m_pTopImageTransform->SetSize(size);
	m_pBottomImageTransform->SetSize(size);
}

// 出現
void BlackBelt::Appear(bool isAppear)
{
	m_moveRate.SetMovement(isAppear ? 1.0f : 0.0f, m_moveTime, Easing::Type::Quart, Easing::InOut::Out);
}

// 幅を設定
void BlackBelt::SetWidth(float width)
{
	Math::Vector2 size = m_pTopImageTransform->GetRect().size;
	size.x = width;
	m_pTopImageTransform->SetSize(size);
	m_pBottomImageTransform->SetSize(size);
}
