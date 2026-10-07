/*
 * FileName:     InstancingEffectFactory.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/07
 *
 * インスタンシング描画用のエフェクトファクトリ
 */

#pragma once

namespace Renderings
{
	class InstancingEffectFactory final : public DirectX::IEffectFactory
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		InstancingEffectFactory(ID3D11Device* pDevice);

		// エフェクトを作成
		std::shared_ptr<DirectX::IEffect> CreateEffect
		(
			const EffectInfo& info,
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


	private:


		/* メンバ変数 */

		// エフェクト保持用
		std::vector<std::shared_ptr<DirectX::BasicEffect>> m_effects;

		// ディレクトリ
		std::wstring m_directory;

		// デバイス
		ID3D11Device* m_pDevice;

	};
}
