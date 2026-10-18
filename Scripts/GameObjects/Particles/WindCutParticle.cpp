/*
 * FileName:     WindCutParticle.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/18
 *
 * 着地パーティクル
 */

#include "Pch.h"
#include "WindCutParticle.h"

#include "Scripts/Commons/Components/Transform.h"

 // コンストラクタ
WindCutParticle::WindCutParticle(Renderings::IModel3DRenderer* pIModelRenderer, const Systems::IResources& iResources)
	: Particle{ pIModelRenderer, iResources }
	, m_position{}
	, m_length{}
	, m_pUpdateElement{}
{
}

// 初期化処理
void WindCutParticle::Initalize(const Transform* pTransform)
{
	Math::Quaternion rotation = pTransform->GetRotation() * Math::Euler
	{
		Random::GetFloatRange(-10.0f, 10.0f),
		Random::GetFloatRange(-10.0f, 10.0f),
		Random::GetFloatRange(-10.0f, 10.0f)
	}.CreateQuaternion();

	Math::Vector3 direction = Math::Vector3::Transform(Math::Vector3::Backward, rotation);

	Math::Vector3 position = pTransform->GetPosition() + Math::Vector3
	{
		Random::GetFloatRange(-0.75f, 0.75f),
		Random::GetFloatRange(-0.75f, 0.75f),
		Random::GetFloatRange(-0.75f, 0.75f)
	};

	m_position.SetMovement(position, position + direction * 2.5f, 0.25f, Easing::Type::Sine, Easing::InOut::InOut);
	m_length.SetMovement(-1.0f, 1.0f, 0.25f, Easing::Type::Sine, Easing::InOut::InOut);

	// 描画情報
	RenderElement* pRenderElement{};
	AddElement(&m_pUpdateElement, &pRenderElement);

	m_pUpdateElement->position = m_position.GetMovement();
	m_pUpdateElement->rotation = rotation;
	m_pUpdateElement->scale = Math::Vector3{ 0.05f, 0.05f, 0.0f };
	// モデルを設定
	SetModelSource("Box", pRenderElement);
	ApplyTransform();
}

// 更新処理
void WindCutParticle::Update(float elapsedTime)
{
	// 移動が終わったら消す
	if (!m_position.IsMoving())
	{
		Destroy();
		return;
	}

	// 速さの更新
	m_position.Tick(elapsedTime);
	// 大きさの更新
	m_length.Tick(elapsedTime);

	m_pUpdateElement->position = m_position.GetMovement();
	m_pUpdateElement->scale.z = (1.0f - Math::Abs(m_length.GetMovement())) * 2.5f;

	ApplyTransform();
}
