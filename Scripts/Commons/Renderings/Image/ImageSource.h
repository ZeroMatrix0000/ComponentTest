/*
 * FileName:     ImageSource.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/24
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
		void Initialize(Ref<ID3D11Device5> deviceRef, const std::wstring& filePath);

		// シェーダーリソースビューを取得
		auto* GetTexture()  const { return m_texture.Get(); }
		// リソースを取得
		auto* GetResource() const { return m_resource.Get(); }

		// 初期化済みかどうかを取得
		bool IsInitialized() const { return m_texture.Get() && m_resource.Get(); }


		/* 静的関数 */

		// 生成
		static ImageSource Create(Ref<ID3D11Device5> deviceRef, const std::wstring& filePath);


	private:


		/* メンバ変数 */

		// シェーダーリソースビュー
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
		// リソース
		Microsoft::WRL::ComPtr<ID3D11Texture2D>          m_resource;

	};
}
