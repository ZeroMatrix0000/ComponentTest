/*
 * FileName:     JumpingParticle.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/21
 *
 * ジャンプパーティクル
 */

#pragma once

#include "Scripts/Commons/Renderings/Particle/Particle.h"

// ジャンプパーティクル
class JumpingParticle : public Renderings::Particle
{

public:


	/* メンバ関数 */

	// コンストラクタ
	JumpingParticle(Renderings::IModel3DRenderer* pIModelRenderer, Systems::IResources* pIResources);

	// 初期化処理
	void Initalize(const Transform& transform) override;

	// 更新処理
	void Update(float elapsedTime) override;


private:


	/* 定数 */

	// 要素数
	static constexpr int ELEMENT_COUNT = 8;


	/* メンバ変数 */

	// 方向
	std::vector<Math::Vector3> m_directions;
	// 速さ
	Easing::Value<float> m_speed;
	// 大きさ
	Easing::Value<Math::Vector3> m_scale;

};
