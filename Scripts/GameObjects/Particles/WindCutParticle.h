/*
 * FileName:     WindCutParticle.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/18
 *
 * 弾けパーティクル
 */

#pragma once

#include "Scripts/Commons/Renderings/Particle/Particle.h"

 // 弾けパーティクル
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
	WindCutParticle(Renderings::IModel3DRenderer* pIModelRenderer, const Systems::IResources& iResources);

	// 初期化処理
	void Initalize(const Transform* pTransform);

	// 更新処理
	void Update(float elapsedTime);


private:


	/* メンバ変数 */

	// 速さ
	Easing::Value<Math::Vector3> m_position;
	// 大きさ
	Easing::Value<float> m_length;

	// 更新情報
	UpdateElement* m_pUpdateElement;

};
