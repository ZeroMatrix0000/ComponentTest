/*
 * FileName:     SceneTransitionAnimation.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/09/28
 *
 * シーン遷移時のアニメーション
 */

#include "Pch.h"
#include "SceneTransitionAnimation.h"

#include "../Renderings/Canvas.h"
#include "../Renderings/Image/Image.h"
#include "../GameObjects/GameObject.h"
#include "../GameObjects/IGameObjectInstantiator.h"
#include "../Components/RectTransform.h"
#include "../Components/IComponentManager.h"

// コンストラクタ
Scenes::SceneTransitionAnimation::SceneTransitionAnimation()
	: m_angle{}
	, m_changingRate{}
	, m_pImageRectTransform{}
	, m_pCanvas{}
{
}

// 初期化処理
void Scenes::SceneTransitionAnimation::Initialize(const Math::Vector2& outputSize, IGameObjectInstantiator* pIGameObjectInstantiator)
{
	// アニメーションの設定
	SetAnimation(false);

	// 画像の設定
	auto* pObj = pIGameObjectInstantiator->Instantiate("Prefab_SceneTransitionAnimation", true);
	m_pImageRectTransform = pObj->GetComponent<RectTransform>();
	auto* pImage = pObj->GetComponent<Renderings::Image>();
	// キャンバスの設定
	m_pCanvas = pIGameObjectInstantiator->Instantiate("Prefab_Canvas", true)->GetComponent<Renderings::Canvas>();
	m_pCanvas->Initialize(Renderings::Canvas::FixedSize::None, outputSize);

	// キャンバスに画像を映す
	pImage->SetCanvas(*m_pCanvas);

	m_angle = m_pImageRectTransform->GetAngle();

	// 長方形サイズの設定
	m_pImageRectTransform->SetSize(Math::Vector2
	{
		Math::Abs(outputSize.x * Math::Cos(Math::Deg2Rad(m_angle))) + Math::Abs(outputSize.y * Math::Sin(Math::Deg2Rad(m_angle))),
		Math::Abs(outputSize.x * Math::Sin(Math::Deg2Rad(m_angle))) + Math::Abs(outputSize.y * Math::Cos(Math::Deg2Rad(m_angle)))
	});
}

// 更新処理
void Scenes::SceneTransitionAnimation::Update(float elapsedTime)
{
	// 切り替え率を更新
	m_changingRate.Tick(elapsedTime);

	// 移動方向
	Math::Vector2 direction = Math::Vector2{ Math::Cos(Math::Deg2Rad(m_angle)), Math::Sin(Math::Deg2Rad(m_angle)) } * m_pImageRectTransform->GetRect().size.x;
	// 長方形座標の設定
	m_pImageRectTransform->SetPosition(direction * m_changingRate.GetMovement());
}

// キャンバスサイズの変更
void Scenes::SceneTransitionAnimation::SetCanvasSize(const Math::Vector2& outputSize)
{
	m_pCanvas->SetSize(outputSize);

	// 長方形サイズの設定
	m_pImageRectTransform->SetSize(Math::Vector2
		{
			Math::Abs(outputSize.x * Math::Cos(Math::Deg2Rad(m_angle))) + Math::Abs(outputSize.y * Math::Sin(Math::Deg2Rad(m_angle))),
			Math::Abs(outputSize.x * Math::Sin(Math::Deg2Rad(m_angle))) + Math::Abs(outputSize.y * Math::Cos(Math::Deg2Rad(m_angle)))
		});

	// 移動方向
	Math::Vector2 direction = Math::Vector2
	{
		Math::Cos(Math::Deg2Rad(m_angle)),
		Math::Sin(Math::Deg2Rad(m_angle))
	} * m_pImageRectTransform->GetRect().size.x;
	// 長方形座標の設定
	m_pImageRectTransform->SetPosition(direction* m_changingRate.GetMovement());
}

// 切り替え率を設定
void Scenes::SceneTransitionAnimation::SetAnimation(bool isEnter)
{
	m_changingRate.SetMovement
	(
		isEnter ? -1.0f : 0.0f,
		isEnter ? 0.0f : 1.0f,
		CHANGE_TIME,
		Easing::Type::Sine,
		isEnter ? Easing::InOut::Out : Easing::InOut::In
	);
}
