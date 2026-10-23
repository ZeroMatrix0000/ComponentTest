/*
 * FileName:     BlackBelt.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/23
 *
 * 上下の黒帯
 */

#include "Pch.h"
#include "BlackBelt.h"

#include "Scripts/Commons/Components/RectTransform.h"
#include "Scripts/Commons/Renderings/Canvas.h"
#include "Scripts/Commons/Renderings/Image/Image.h"
#include "Scripts/Commons/Systems/JsonSerializer.h"
#include "Scripts/Commons/Systems/IsDerivedRegister.h"

// コンストラクタ
BlackBelt::BlackBelt(const ComponentDesc& desc)
	: Component{ desc }
	, m_moveTime{}
	, m_height{}
	, m_moveRate{}
	, m_refTopImageTransform{ GetOwner()->GetNullReference<RectTransform>() }
	, m_refTopImage{ GetOwner()->GetNullReference<Renderings::Image>() }
	, m_refBottomImageTransform{ GetOwner()->GetNullReference<RectTransform>() }
	, m_refBottomImage{ GetOwner()->GetNullReference<Renderings::Image>() }
{
}

// 初期化処理
void BlackBelt::Initalize(const nlohmann::ordered_json& json, Ref<IGameObjectFinder> iGameObjectFinder)
{
	m_moveRate.SetMovement(0.0f, 0.0f);

	const Renderings::Canvas* pCanvas = nullptr;

	Systems::JsonSerializer serializer{ iGameObjectFinder };
	serializer.AddParameter(&m_moveTime, "MoveTime");
	serializer.AddParameter(&pCanvas, "Canvas");
	serializer.Load(json);

	// 画像を生成
	if (m_refTopImageTransform == GetOwner()->GetNullReference<RectTransform>())
	{
		Ref<GameObject> refObj = Instantiate("Prefab_BlackBeltImage");
		m_refTopImageTransform = refObj->GetComponent<RectTransform>();
		m_refTopImage = refObj->GetComponent<Renderings::Image>();
		m_refTopImageTransform->SetAnchor(Utility::AlignmentPoint::TopCenter);
		refObj = Instantiate("Prefab_BlackBeltImage");
		m_refBottomImageTransform = refObj->GetComponent<RectTransform>();
		m_refBottomImage = refObj->GetComponent<Renderings::Image>();
		m_refBottomImageTransform->SetAnchor(Utility::AlignmentPoint::BottomCenter);
		
		// 高さを取得
		m_height = m_refTopImageTransform->GetRect().size.y;
	}

	// キャンバスを設定
	if (pCanvas)
	{
		m_refTopImage->SetCanvas(*pCanvas);
		m_refBottomImage->SetCanvas(*pCanvas);
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
	Math::Color color = m_refTopImage->GetColor();
	color.w = moveRate;
	m_refTopImage->SetColor(color);
	m_refBottomImage->SetColor(color);

	// 移動率が 0 なら以降何もしない
	if (m_moveRate.GetMovement() == 0.0f)
	{
		return;
	}

	// サイズを変更
	Math::Vector2 size = m_refTopImageTransform->GetRect().size;
	size.y = m_height * moveRate;
	m_refTopImageTransform->SetSize(size);
	m_refBottomImageTransform->SetSize(size);
}

// 出現
void BlackBelt::Appear(bool isAppear)
{
	m_moveRate.SetMovement(isAppear ? 1.0f : 0.0f, m_moveTime, Easing::Type::Quart, Easing::InOut::Out);
}

// 幅を設定
void BlackBelt::SetWidth(float width)
{
	Math::Vector2 size = m_refTopImageTransform->GetRect().size;
	size.x = width;
	m_refTopImageTransform->SetSize(size);
	m_refBottomImageTransform->SetSize(size);
}
