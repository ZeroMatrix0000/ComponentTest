/*
 * FileName:     WindCutParticle.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/21
 *
 * 風切りパーティクル
 */

#pragma once

#include "Scripts/Commons/Renderings/Particle/Particle.h"

 // 風切りパーティクル
class WindCutParticle : public Renderings::Particle
{

public:


	/* 定数 */

	// 出現間隔
	static constexpr float INTERVAL = 0.02f;
	// 出現速度
	static constexpr float SPEED = 16.0f;


	/* メンバ関数 */

	// コンストラクタ
	WindCutParticle(Renderings::IModel3DRenderer* pIModelRenderer, Systems::IResources* pIResources);

	// 初期化処理
	void Initalize(const Transform& transform) override;

	// 更新処理
	void Update(float elapsedTime) override;


private:


	/* メンバ変数 */

	// 速さ
	Easing::Value<Math::Vector3> m_position;
	// 大きさ
	Easing::Value<float> m_length;

	// 更新情報
	UpdateElement* m_pUpdateElement;

};
