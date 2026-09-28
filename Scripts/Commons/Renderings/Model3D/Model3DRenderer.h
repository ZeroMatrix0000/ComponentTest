/*
 * FileName:     Model3DRenderer.h
 * Author:       Takao Hayata
 * Last Updated: 2026/09/23
 *
 * 3Dモデル描画
 */

#pragma once

#include "IModel3DRenderer.h"

namespace Systems
{
	class IResources;
}

namespace Renderings
{
	// 3Dモデル描画
	class Model3DRenderer : public IModel3DRenderer
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		Model3DRenderer();

		// 初期化処理
		void Initialize(ID3D11DeviceContext4* pContext, const DirectX::CommonStates& commonStates);
		// 描画処理
		void Render();

		// モデルのポインタを追加
		void AddPModel(const Model3D* pModel) override;
		// モデルのポインタを削除
		void RemovePModel(const Model3D* pModel) override;


	private:


		/* メンバ変数 */

		// モデルのポインタリスト
		std::vector<const Model3D*> m_pModels;

		// コンテキストのポインタ
		ID3D11DeviceContext4* m_pContext;
		// コモンステートのポインタ
		const DirectX::CommonStates* m_pCommonStates;

	};
}
