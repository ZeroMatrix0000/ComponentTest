/*
 * FileName:     ImageSource.h
 * Author:       Takao Hayata
 * Last Updated: 2026/07/06
 *
 * 画像ソース
 */

#pragma once

namespace Renderings
{
	// 画像ソース
	class ImageSource
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		ImageSource();

		// 初期化処理
		void Initialize(ID3D11Device5* device, const std::wstring& filePath);

		// シェーダーリソースビューを取得
		auto* GetTexture()  const { return m_texture.Get(); }
		// リソースを取得
		auto* GetResource() const { return m_resource.Get(); }


		/* 静的関数 */

		// 生成
		static ImageSource Create(ID3D11Device5* device, const std::wstring& filePath);


	private:


		/* メンバ変数 */

		// シェーダーリソースビュー
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
		// リソース
		Microsoft::WRL::ComPtr<ID3D11Texture2D>          m_resource;

	};
}
