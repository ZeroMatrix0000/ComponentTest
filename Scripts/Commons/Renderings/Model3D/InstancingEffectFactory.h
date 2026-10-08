/*
 * FileName:     InstancingEffectFactory.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/08
 *
 * インスタンス描画用のエフェクトファクトリ
 */

#pragma once

namespace Renderings
{
	class Model3DSource;

	class InstancingEffectFactory : public DirectX::IEffectFactory
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		InstancingEffectFactory(ID3D11Device* pDevice);

		// エフェクトを作成
		std::shared_ptr<DirectX::IEffect> CreateEffect
		(
			const EffectInfo& effectInfo,
			ID3D11DeviceContext* pContext
		) override;

		// テクスチャを作成
		void CreateTexture
		(
			const wchar_t* name,
			ID3D11DeviceContext* pContext,
			ID3D11ShaderResourceView** ppTextureView
		) override;

		// ディレクトリを設定
		void SetDirectory(const std::wstring& filePath) { m_directory = filePath; };

		// モデルソースを設定
		void SetModel3DSource(Model3DSource* pModelSource) { m_pModelSource = pModelSource; }


	private:


		/* メンバ変数 */

		// ディレクトリ
		std::wstring m_directory;

		// モデルソース
		Model3DSource* m_pModelSource;

		// デバイス
		ID3D11Device* m_pDevice;

	};
}
