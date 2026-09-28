/*
 * FileName:     DecorationString.h
 * Author:       Takao Hayata
 * Last Updated: 2026/09/28
 *
 * デコレーション付き文字列
 */

#include "Pch.h"
#include "DecorationString.h"

// コンストラクタ
Renderings::DecorationString::DecorationString()
	: m_sourceStr{}
	, m_str{}
{
}

// 文字列を設定
void Renderings::DecorationString::SetStr(const std::wstring& str)
{
	m_sourceStr = str;
	m_str.clear();

	// 検索番号
	size_t index = 0;

	while (true)
	{
		// < を探す
		size_t ltIndex = m_sourceStr.find(L"<", index);
		// 見つからなければ終了
		if (ltIndex == std::wstring::npos)
		{
			m_str.append(m_sourceStr.substr(index));
			break;
		}

		// < より前の文字を足す
		m_str.append(m_sourceStr.substr(index, ltIndex - index));

		// > を探す
		size_t gtIndex = m_sourceStr.find(L">", ltIndex);
		// 見つからなければ終了
		if (gtIndex == std::wstring::npos)
		{
			break;
		}

		CalcTag(m_sourceStr.substr(ltIndex + 1, gtIndex - ltIndex - 1));

		// > 以前は無視する
		index = gtIndex + 1;
	}
}

// タグの計算
void Renderings::DecorationString::CalcTag(const std::wstring& tag)
{
	// 色の設定
	if (tag.substr(0, 7) == L"color=#")
	{


		// カラーコード
		auto colorCode = std::stoul(tag.substr(7), nullptr, 16);
		auto color = D2D1::ColorF(colorCode % 0x1000000, colorCode / 0x1000000 / 255.0f);
		color = color;
	}
}
