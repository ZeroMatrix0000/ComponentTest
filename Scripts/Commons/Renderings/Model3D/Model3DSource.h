/*
 * FileName:     Model3DSource.h
 * Author:       Takao Hayata
 * Last Updated: 2026/07/06
 *
 * 3Dモデルのソース
 */

#pragma once

namespace Renderings
{
	// 3Dモデルのソース
	class Model3DSource
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		Model3DSource();

		// 初期化処理
		void Initialize(ID3D11Device* device, DirectX::EffectFactory* fx, const std::wstring& filePath);

		// モデルを取得
		const auto& GetModel() const
		{
			return *m_model;
		}


		/* 静的関数 */

		// 生成
		static Model3DSource Create(ID3D11Device* device, DirectX::EffectFactory* fx, const std::wstring& filePath);


	private:


		/* メンバ変数 */

		// モデル
		std::unique_ptr<DirectX::Model> m_model;

	};
}
