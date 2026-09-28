/*
 * FileName:     ITextRenderer.h
 * Author:       Takao Hayata
 * Last Updated: 2026/09/28
 *
 * テキスト描画のインタフェース
 */

#pragma once

#include "Scripts/Commons/Systems/OnlyOne.h"

namespace Renderings
{
	class Text;

	// テキスト描画のインタフェース
	class ITextRenderer : public Systems::OnlyOne
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		ITextRenderer()
			: OnlyOne{ typeid(ITextRenderer) }
		{
		}
		// デストラクタ
		virtual ~ITextRenderer() = default;

		// テキストのポインタを追加
		virtual void AddPText(const Text* pText) = 0;
		// テキストのポインタを削除
		virtual void RemovePText(const Text* pText) = 0;

	};
}
