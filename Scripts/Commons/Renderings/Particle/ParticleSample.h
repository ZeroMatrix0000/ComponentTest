/*
 * FileName:     ParticleSample.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/09
 *
 * パーティクルのサンプル
 */

#pragma once

#include "Particle.h"

namespace Renderings
{
	// 3Dモデル
	class ParticleSample : public Particle
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		ParticleSample(IModel3DRenderer* pIModelRenderer, const Systems::IResources& iResources);

		// 初期化処理
		void Initalize(const Transform* pTransform);

		// 更新処理
		void Update(float elapsedTime);


	private:


		/* 定数 */

		// 要素数
		static constexpr int ELEMENT_COUNT = 8;


		/* メンバ変数 */

		// 速度
		std::vector<Math::Vector3> m_velocities;

		// アクティブタイマー
		float m_isActiveTimer;

	};
}
