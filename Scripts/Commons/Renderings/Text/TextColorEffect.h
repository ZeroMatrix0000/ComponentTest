/*
 * FileName:     TextColorEffect.h
 * Author:       Takao Hayata
 * Last Updated: 2026/09/28
 *
 * テキスト色付けエフェクト
 */

#pragma once

#include "ITextRenderer.h"
#include "DecorationTextRenderer.h"

namespace Renderings
{
	// テキスト色付けエフェクト
	class TextColorEffect : public IUnknown
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		TextColorEffect(const D2D1::ColorF& color);


	private:


		/* メンバ変数 */

		// 色
		D2D1::ColorF m_color;

		// 参照数
		ULONG m_refCount = 1;

	};
}
