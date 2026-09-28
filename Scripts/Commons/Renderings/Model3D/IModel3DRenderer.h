/*
 * FileName:     IModel3DRenderer.h
 * Author:       Takao Hayata
 * Last Updated: 2026/09/28
 *
 * 3Dモデル描画のインタフェース
 */

#pragma once

#include "Scripts/Commons/Systems/OnlyOne.h"

namespace Renderings
{
	class Model3D;

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
		virtual void AddPModel(const Model3D* pModel) = 0;
		// モデルのポインタを削除
		virtual void RemovePModel(const Model3D* pModel) = 0;

	};
}
