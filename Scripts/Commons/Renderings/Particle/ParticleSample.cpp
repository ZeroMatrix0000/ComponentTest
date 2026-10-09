/*
 * FileName:     ParticleSample.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/09
 *
 * パーティクルのサンプル
 */

#include "Pch.h"
#include "ParticleSample.h"

#include "Scripts/Commons/Components/Transform.h"

// コンストラクタ
Renderings::ParticleSample::ParticleSample(IModel3DRenderer* pIModelRenderer, const Systems::IResources& iResources)
	: Particle{ pIModelRenderer, iResources }
{
}

// 初期化処理
void Renderings::ParticleSample::Initalize(const Transform* pTransform)
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
		pUpdateElement->rotation = Math::Quaternion::CreateFromYawPitchRoll
		(
			Random::GetFloatRange(0.0f, 360.0f),
			Random::GetFloatRange(0.0f, 360.0f),
			Random::GetFloatRange(0.0f, 360.0f)
		);
		// スケールを 0.5 倍
		pUpdateElement->scale = Math::Vector3::One * 0.5f;

		// モデルを設定
		SetModelSource("Box", pRenderElement);

		// 速度を設定
		m_velocities.push_back(Math::Vector3::Transform
		(
			Math::Vector3::Forward * 2.0f,
			Math::Quaternion::CreateFromAxisAngle(Math::Vector3::Up, Math::Deg2Rad(static_cast<float>(i) * 45.0f))
		));
	}
	ApplyTransform();
}

// 更新処理
void Renderings::ParticleSample::Update(float elapsedTime)
{
	// 1 秒経ったら消す
	m_isActiveTimer += elapsedTime;
	if (m_isActiveTimer >= 1.0f)
	{
		Destroy();
	}

	for (int i = 0; i < ELEMENT_COUNT; i++)
	{
		GetPUpdateElement(i)->position += m_velocities.at(i) * elapsedTime;
	}
		
	ApplyTransform();
}
