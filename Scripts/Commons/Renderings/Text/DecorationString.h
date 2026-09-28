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

		// 元の文字列を取得
		const std::wstring& GetSourceStr() const { return m_sourceStr; }

		// 文字列を取得
		const std::wstring& GetStr() const { return m_str; }

		// 文字列を設定
		void SetStr(const std::wstring& str);


	private:


		/* メンバ関数 */

		// タグの計算
		void CalcTag(const std::wstring& tag);


		/* メンバ変数 */

		// 元の文字列
		std::wstring m_sourceStr;

		// 文字列
		std::wstring m_str;

	};
}
