/*
 * FileName:     ImageRenderer.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/24
 *
 * 画像描画
 */

#pragma once

#include "IImageRenderer.h"

namespace Systems
{
	class IResources;
}

namespace Renderings
{
	// 画像描画
	class ImageRenderer : public IImageRenderer
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		ImageRenderer();

		// 初期化処理
		void Initialize
		(
			Ref<ID3D11Device5> deviceRef,
			Ref<ID3D11DeviceContext4> contextRef,
			Ref<const DirectX::CommonStates> commonStatesRef
		);

		// 描画開始
		void Begin();
		// 描画処理
		void Draw(Ref<const Image> imageRef);
		// 描画終了
		void End();

		// 画像の参照を追加
		void AddImageRef(Ref<const Image> imageRef) override;
		// 画像の参照を削除
		void RemoveImageRef(Ref<const Image> imageRef) override;

		// 画像の大きさを取得
		Math::Vector2 GetImageSize(Ref<const Image> imageRef) const override;

		// 画像のポインタリストのソート
		void SortPImages();

		// 画像の参照リストを取得
		const std::vector<Ref<const Image>> GetImageRefs() const { return m_imageRefs; }


	private:


		/* メンバ変数 */

		// スプライトバッチ
		std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;

		// 画像の参照リスト
		std::vector<Ref<const Image>> m_imageRefs;

		// デバイス
		ID3D11Device5* m_pDevice;
		// デバイスコンテキスト
		ID3D11DeviceContext4* m_pContext;

		// コモンステート
		const DirectX::CommonStates* m_pCommonStates;

	};
}
