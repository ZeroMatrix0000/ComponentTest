/*
 * FileName:     DecorationString.h
 * Author:       Takao Hayata
 * Last Updated: 2026/09/28
 *
 * デコレーション付き文字列
 */

#pragma once

#include "Scripts/Commons/Components/Component.h"

namespace Renderings
{
	// デコレーション付き文字列
	class DecorationString
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		DecorationString();


	private:


		/* メンバ変数 */

		// 文字列
		std::wstring m_str;

	};
}
