/*
 * FileName:     IImageRenderer.h
 * Author:       Takao Hayata
 * Last Updated: 2026/09/28
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

		// 画像のポインタを追加
		virtual void AddPImage(const Image* pImage) = 0;
		// 画像のポインタを削除
		virtual void RemovePImage(const Image* pImage) = 0;

		// 画像の大きさを取得
		virtual Math::Vector2 GetImageSize(const Image* pImage) const = 0;

	};
}
