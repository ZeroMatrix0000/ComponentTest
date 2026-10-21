/*
 * FileName:     ParticleSample.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/21
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
		ParticleSample(IModel3DRenderer* pIModelRenderer, Systems::IResources* pIResources);

		// 初期化処理
		void Initalize(const Transform& transform) override;

		// 更新処理
		void Update(float elapsedTime) override;


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
