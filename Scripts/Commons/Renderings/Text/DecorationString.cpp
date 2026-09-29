/*
 * FileName:     DecorationString.h
 * Author:       Takao Hayata
 * Last Updated: 2026/09/29
 *
 * デコレーション付き文字列
 */

#include "Pch.h"
#include "DecorationString.h"

#include "Scripts/Commons/Systems/IErrorMessage.h"

// コンストラクタ
Renderings::DecorationString::DecorationString()
	: m_sourceStr{}
	, m_str{}
	, m_colorDescList{}
	, m_colorDescCount{}
{
}

// 文字列を設定
void Renderings::DecorationString::SetStr(const std::wstring& str, bool isDecoration)
{
	m_sourceStr = str;
	m_str.clear();
	m_colorDescList.clear();
	m_colorDescCount = 0;

	if (!isDecoration)
	{
		m_str = str;
		return;
	}

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

	// 確定してない色付け詳細を確定させる
	for (size_t i = m_colorDescCount; i < m_colorDescList.size(); i++)
	{
		m_colorDescList.at(i).length = m_str.size() - m_colorDescList.at(m_colorDescCount).beginIndex;
	}
}

// タグの計算
void Renderings::DecorationString::CalcTag(const std::wstring& tag)
{
	try
	{
		// 色の設定
		if (tag.substr(0, 7) == L"color=#")
		{
			D2D1::ColorF color{ 0 };

			std::wstring str = tag.substr(7);
			if (str.size() == 6)
			{
				// カラーコード
				auto colorCode = std::stoul(str, nullptr, 16);
				color = D2D1::ColorF(colorCode, 1.0f);
			}
			else if (str.size() == 8)
			{
				// カラーコード
				auto colorCode = std::stoul(str, nullptr, 16);
				color = D2D1::ColorF(colorCode % 0x1000000, colorCode / 0x1000000 / 255.0f);
			}

			m_colorDescList.push_back(ColorDesc{});
			m_colorDescList.back().effect.Attach(new TextColorEffect{ color });
			m_colorDescList.back().beginIndex = m_str.size();
		}
		// 色の設定の終了
		else if (tag == L"/color")
		{
			m_colorDescList.at(m_colorDescCount).length = m_str.size() - m_colorDescList.at(m_colorDescCount).beginIndex;
			m_colorDescCount++;
		}
		else
		{
			// エラーメッセージを追加
			Systems::IErrorMessage::GetInstance()->AddMessage(Utility::FormatWString
			(
				L"タグの処理に失敗しました。 | tag: <%s>",
				tag.c_str()
			));
		}
	}
	catch (std::exception e)
	{
		// エラーメッセージを追加
		Systems::IErrorMessage::GetInstance()->AddMessage(Utility::FormatWString
		(
			L"タグの処理に失敗しました。 | tag: <%s>",
			tag.c_str()
		));
	}
}
