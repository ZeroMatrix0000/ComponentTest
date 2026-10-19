/*
 * FileName:     SparklingParticle.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/19
 *
 * 着地パーティクル
 */

#include "Pch.h"
#include "SparklingParticle.h"

#include "Scripts/Commons/Components/Transform.h"

 // コンストラクタ
SparklingParticle::SparklingParticle(Renderings::IModel3DRenderer* pIModelRenderer, const Systems::IResources& iResources)
	: Particle{ pIModelRenderer, iResources }
	, m_directions{}
	, m_speed{}
	, m_scale{}
{
}

// 初期化処理
void SparklingParticle::Initalize(const Transform* pTransform)
{
	for (int i = 0; i < ELEMENT_COUNT; i++)
	{
		// 更新情報
		UpdateElement* pUpdateElement{};
		// 描画情報
		RenderElement* pRenderElement{};
		AddElement(&pUpdateElement, &pRenderElement);

		// 管理者の位置に移動
		pUpdateElement->position = pTransform->GetPosition();
		// ランダムに回転
		pUpdateElement->rotation = Math::Euler
		{
			Random::GetFloatRange(0.0f, 360.0f),
			Random::GetFloatRange(0.0f, 360.0f),
			Random::GetFloatRange(0.0f, 360.0f)
		}.CreateQuaternion();
		// 拡大
		pUpdateElement->scale = Math::Vector3::One * 0.2f;

		// モデルを設定
		SetModelSource("Box", pRenderElement);

		// 方向
		Math::Vector3 direction = Math::Vector3
		{
			Random::GetFloatRange(-1.0f, 1.0f),
			Random::GetFloatRange(-1.0f, 1.0f),
			Random::GetFloatRange(-1.0f, 1.0f)
		};
		direction.Normalize();
		// 方向を設定
		m_directions.push_back(Math::Vector3::Transform
		(
			Math::Vector3::Forward,
			Math::Quaternion::CreateFromAxisAngle
			(
				direction,
				Math::Deg2Rad(Random::GetFloatRange(-180.0f, 180.0f))
			)
		));

		pRenderElement->color = Math::Color{ 1.0f, 0.8f, 0.5f, 1.0f };
	}
	ApplyTransform();

	m_speed.SetMovement(5.0f, 0.0f, 0.75f, Easing::Type::Sine, Easing::InOut::Out);
	m_scale.SetMovement(Math::Vector3::One * 0.2f, Math::Vector3::Zero, 0.75f, Easing::Type::Sine, Easing::InOut::In);
}

// 更新処理
void SparklingParticle::Update(float elapsedTime)
{
	// 移動が終わったら消す
	if (!m_speed.IsMoving())
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
