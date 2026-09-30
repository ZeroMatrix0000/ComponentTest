/*
 * FileName:     DecorationString.h
 * Author:       Takao Hayata
 * Last Updated: 2026/09/30
 *
 * デコレーション付き文字列
 */

#pragma once

#include "TextColorEffect.h"
#include "Scripts/Commons/Components/Component.h"

namespace Renderings
{
	class TextColorEffect;

	// デコレーション付き文字列
	class DecorationString
	{

	public:


		/* 構造体 */

		// 色付け詳細
		struct ColorDesc
		{
			// 色付けエフェクト
			Microsoft::WRL::ComPtr<TextColorEffect> effect{};

			// 開始文字番号
			size_t beginIndex{};
			// 文字数
			size_t length{};
		};

		// ルビ詳細
		struct RubyDesc
		{
			// ルビの文字列
			std::wstring m_text{};

			// 開始文字番号
			size_t beginIndex{};
			// 文字数
			size_t length{};
		};


		/* メンバ関数 */

		// コンストラクタ
		DecorationString();

		// 文字列を設定
		void SetStr(const std::wstring& str);
		// デコレーションするかどうかを設定
		void SetIsDecoration(bool isDecoration) { m_isDecoration = isDecoration; }

		// 元の文字列を取得
		const std::wstring& GetSourceStr() const { return m_sourceStr; }
		// 文字列を取得
		const std::wstring& GetStr() const { return m_str; }
		// 文字列を取得
		bool IsDecoration() const { return m_isDecoration; }

		// 色付け詳細リストを取得
		const std::vector<ColorDesc>& GetColorDescList() const { return m_colorDescList; };
		// ルビ詳細リストを取得
		const std::vector<RubyDesc>& GetRubyDescList() const { return m_rubyDescList; };


	private:


		/* メンバ関数 */

		// タグの計算
		void CalcTag(const std::wstring& tag);


		/* メンバ変数 */

		// 元の文字列
		std::wstring m_sourceStr;
		// 文字列
		std::wstring m_str;

		// デコレーションするかどうか
		bool m_isDecoration;

		// 色付け詳細リスト
		std::vector<ColorDesc> m_colorDescList;
		// 色付け詳細リストの確定数
		size_t m_colorDescCount;

		// ルビ詳細リスト
		std::vector<RubyDesc> m_rubyDescList;
		// ルビ詳細リストの確定数
		size_t m_rubyDescCount;

	};
}
