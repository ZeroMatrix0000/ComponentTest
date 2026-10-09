/*
 * FileName:     LandingParticle.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/09
 *
 * 着地パーティクル
 */

#include "Pch.h"
#include "LandingParticle.h"

#include "Scripts/Commons/Components/Transform.h"

 // コンストラクタ
LandingParticle::LandingParticle(Renderings::IModel3DRenderer* pIModelRenderer, const Systems::IResources& iResources)
	: Particle{ pIModelRenderer, iResources }
	, m_directions{}
	, m_speed{}
	, m_scale{}
	, m_isActiveTimer{}
{
}

// 初期化処理
void LandingParticle::Initalize(const Transform* pTransform)
{
	for (int i = 0; i < ELEMENT_COUNT; i++)
	{
		// 更新情報
		UpdateElement* pUpdateElement{};
		// 描画情報
		RenderElement* pRenderElement{};
		AddElement(&pUpdateElement, &pRenderElement);

		// 管理者の位置に移動
		pUpdateElement->position = pTransform->GetPosition() + Math::Vector3{ 0.0f, 0.1f, 0.0f };
		// ランダムに回転
		pUpdateElement->rotation = Math::Quaternion::CreateFromYawPitchRoll
		(
			Random::GetFloatRange(0.0f, 360.0f),
			Random::GetFloatRange(0.0f, 360.0f),
			Random::GetFloatRange(0.0f, 360.0f)
		);
		// スケールを 0.5 倍
		pUpdateElement->scale = Math::Vector3::One * 0.25f;

		// モデルを設定
		SetModelSource("Box", pRenderElement);

		// 方向を設定
		m_directions.push_back(Math::Vector3::Transform
		(
			Math::Vector3::Forward * Random::GetFloatRange(0.8f, 1.0f),
			Math::Quaternion::CreateFromAxisAngle
			(
				Math::Vector3::Up,
				Math::Deg2Rad(i * 360.0f / ELEMENT_COUNT + Random::GetFloatRange(-10.0f, 10.0f))
			)
		));
	}
	ApplyTransform();

	m_speed.SetMovement(5.0f, 0.0f, 0.5f, Easing::Type::Sine, Easing::InOut::Out);
	m_scale.SetMovement(Math::Vector3::One * 0.25f, Math::Vector3::Zero, 0.5f, Easing::Type::Sine, Easing::InOut::In);
}

// 更新処理
void LandingParticle::Update(float elapsedTime)
{
	// 1 秒経ったら消す
	m_isActiveTimer += elapsedTime;
	if (m_isActiveTimer >= 0.5f)
	{
		Destroy();
		return;
	}

	// 速さの更新
	m_speed.Tick(elapsedTime);
	// 大きさの更新
	m_scale.Tick(elapsedTime);

	for (int i = 0; i < ELEMENT_COUNT; i++)
	{
		// 移動
		GetPUpdateElement(i)->position += m_directions.at(i) * m_speed.GetMovement() * elapsedTime;
		// 拡大
		GetPUpdateElement(i)->scale = m_scale.GetMovement();
	}

	ApplyTransform();
}
