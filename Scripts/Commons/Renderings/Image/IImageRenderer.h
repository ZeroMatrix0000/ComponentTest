/*
 * FileName:     IImageRenderer.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/24
 *
 * 画像描画のインタフェース
 */

#pragma once

#include "Scripts/Commons/Systems/OnlyOne.h"

namespace Renderings
{
	class Image;

	// 画像描画のインタフェース
	class IImageRenderer : public Systems::OnlyOne
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		IImageRenderer()
			: OnlyOne{ typeid(IImageRenderer) }
		{
		}
		// デストラクタ
		virtual ~IImageRenderer() = default;

		// 画像の参照を追加
		virtual void AddImageRef(Ref<const Image> imageRef) = 0;
		// 画像の参照を削除
		virtual void RemoveImageRef(Ref<const Image> imageRef) = 0;

		// 画像の大きさを取得
		virtual Math::Vector2 GetImageSize(Ref<const Image> imageRef) const = 0;

	};
}
