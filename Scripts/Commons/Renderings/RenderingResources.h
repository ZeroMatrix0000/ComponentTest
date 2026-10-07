/*
 * FileName:     RenderingResources.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/07
 *
 * 描画リソース
 */

#pragma once

#include "../Systems/OnlyOne.h"
#include "Model3D/InstancingEffectFactory.h"

namespace Renderings
{
	// 描画リソース
	class RenderingResources : public Systems::OnlyOne
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		RenderingResources();

		// 初期化処理
		void Initialize(ID3D11Device5* device);

		// エフェクトファクトリーのディレクトリを設定
		void SetEffectFactoryDirectory(const std::wstring& filePath);

		// コモンステートの取得
		const auto& GetCommonStates()  const { return *m_commonStates; }
		// エフェクトファクトリーの取得
		auto* GetEffectFactory() const { return m_effectFactory.get(); }


	private:


		/* メンバ変数 */

		// コモンステート
		std::unique_ptr<DirectX::CommonStates>  m_commonStates;
		// エフェクトファクトリー
		std::unique_ptr<InstancingEffectFactory> m_effectFactory;

	};
}
