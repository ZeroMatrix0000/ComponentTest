/*
 * FileName:     ParticleManager.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/09
 *
 * パーティクル管理
 */

#pragma once

#include "Particle.h"
#include "Scripts/Commons/Components/Component.h"

namespace Systems
{
	class IResources;
}

namespace Renderings
{
	class ICameraScreen;
	class IModel3DRenderer;

	// パーティクル管理
	class ParticleManager : public Component
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		ParticleManager(const ComponentDesc& desc, IModel3DRenderer* pIModelRenderer, const Systems::IResources& iResources);

		// 初期化処理
		void Initalize(const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder) override;

		// 更新処理
		void Update(float elapsedTime);

		// 映るカメラ画面を追加
		void AddICameraScreen(const ICameraScreen& iCameraScreen);
		// 映るカメラ画面を削除
		void RemoveICameraScreen(const ICameraScreen& iCameraScreen);

		// パーティクルを生成
		template<typename TParticle> requires IsDerived<TParticle, Particle>
		void Play()
		{
			// パーティクルを生成
			auto& particle = m_particles.emplace_back();
			particle = std::make_unique<TParticle>(m_pIModelRenderer, m_refIResources);
			// カメラを設定
			for (const auto* pICameraScreen : m_pICameraScreens)
			{
				particle->AddICameraScreen(*pICameraScreen);
			}
			// 初期化処理
			particle->Initalize(m_pTransform);
		}


	private:


		/* メンバ変数 */

		// パーティクルリスト
		std::vector<std::unique_ptr<Particle>> m_particles;

		// トランスフォーム
		const Transform* m_pTransform;

		// 映るカメラ画面のポインタリスト
		std::vector<const ICameraScreen*> m_pICameraScreens;

		// モデル描画インタフェースのポインタ
		IModel3DRenderer* m_pIModelRenderer;

		// リソース管理
		const Systems::IResources& m_refIResources;

	};
}
