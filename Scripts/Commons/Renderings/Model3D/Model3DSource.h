/*
 * FileName:     Model3DSource.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/12
 *
 * 3Dモデルのソース
 */

#pragma once

namespace Renderings
{
	class InstancingEffectFactory;
	class PixelShader;

	// 3Dモデルのソース
	class Model3DSource
	{

	public:


		/* 構造体 */

		// パーツごとの情報
		struct PartInfo
		{
			// エフェクト情報
			DirectX::IEffectFactory::EffectInfo effectInfo{};
			// テクスチャ
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture{};
			// 法線マップ
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalMap{};
			// 鏡面反射マップ
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> specularMap{};
		};


		/* メンバ関数 */

		// コンストラクタ
		Model3DSource();

		// 初期化処理
		void Initialize(ID3D11Device* device, InstancingEffectFactory* fx, const std::wstring& filePath);

		// パーツごとの情報を追加
		void AddPartInfo
		(
			const DirectX::IEffectFactory::EffectInfo& effectInfo,
			ID3D11ShaderResourceView* pTexture,
			ID3D11ShaderResourceView* pNormalMap,
			ID3D11ShaderResourceView* pSpecularMap
		);

		// ピクセルシェーダを設定
		void SetPixelShader(const PixelShader* pPixelShader) { m_pPixelShader = pPixelShader; }

		// モデルを取得
		const auto& GetModel() const
		{
			return *m_model;
		}

		// パーツごとの情報を取得
		const PartInfo& GetPartInfo(size_t index) const { return m_partInfos.at(index); };

		// ピクセルシェーダを取得
		const PixelShader* GetPPixelShader() const { return m_pPixelShader; }

		/* 静的関数 */

		// 生成
		static Model3DSource Create(ID3D11Device* device, InstancingEffectFactory* fx, const std::wstring& filePath);


	private:


		/* メンバ変数 */

		// モデル
		std::unique_ptr<DirectX::Model> m_model;

		// パーツごとの情報
		std::vector<PartInfo> m_partInfos;

		// ピクセルシェーダ
		const PixelShader* m_pPixelShader;

	};
}
