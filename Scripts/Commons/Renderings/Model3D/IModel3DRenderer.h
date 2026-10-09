/*
 * FileName:     IModel3DRenderer.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/09
 *
 * 3Dモデル描画のインタフェース
 */

#pragma once

#include "../Particle/Particle.h"
#include "Scripts/Commons/Systems/OnlyOne.h"

namespace Renderings
{
	class Model3D;
	class ICameraScreen;

	// 3Dモデル描画のインタフェース
	class IModel3DRenderer : public Systems::OnlyOne
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		IModel3DRenderer()
			: OnlyOne{ typeid(IModel3DRenderer) }
		{
		}
		// デストラクタ
		virtual ~IModel3DRenderer() = default;

		// モデルのポインタを追加
		virtual void AddPModel(const ICameraScreen* pICameraScreen, const Model3D* pModel) = 0;
		// モデルのポインタを削除
		virtual void RemovePModel(const ICameraScreen* pICameraScreen, const Model3D* pModel) = 0;

		// パーティクルのポインタを追加
		virtual void AddPParticle(const ICameraScreen* pICameraScreen, const Particle::RenderElement* pParticleElement) = 0;
		// パーティクルのポインタを削除
		virtual void RemovePParticle(const ICameraScreen* pICameraScreen, const Particle::RenderElement* pParticleElement) = 0;

	};
}
