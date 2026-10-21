/*
 * FileName:     LandingParticle.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/21
 *
 * 着地パーティクル
 */

#pragma once

#include "Scripts/Commons/Renderings/Particle/Particle.h"

// 着地パーティクル
class LandingParticle : public Renderings::Particle
{

public:


	/* メンバ関数 */

	// コンストラクタ
	LandingParticle(Renderings::IModel3DRenderer* pIModelRenderer, Systems::IResources* pIResources);

	// 初期化処理
	void Initalize(const Transform* pTransform);

	// 更新処理
	void Update(float elapsedTime);


private:


	/* 定数 */

	// 要素数
	static constexpr int ELEMENT_COUNT = 12;


	/* メンバ変数 */

	// 方向
	std::vector<Math::Vector3> m_directions;
	// 速さ
	Easing::Value<float> m_speed;
	// 大きさ
	Easing::Value<Math::Vector3> m_scale;

};
