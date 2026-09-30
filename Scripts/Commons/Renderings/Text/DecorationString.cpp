/*
 * FileName:     DecorationString.h
 * Author:       Takao Hayata
 * Last Updated: 2026/09/30
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
	, m_isDecoration{}
	, m_colorDescList{}
	, m_colorDescCount{}
	, m_rubyDescList{}
	, m_rubyDescCount{}
{
}

// 文字列を設定
void Renderings::DecorationString::SetStr(const std::wstring& str)
{
	m_sourceStr = str;
	m_str.clear();
	m_colorDescList.clear();
	m_colorDescCount = 0;
	m_rubyDescList.clear();
	m_rubyDescCount = 0;

	if (!m_isDecoration)
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

	// 確定してない詳細を確定させる
	for (size_t i = m_colorDescCount; i < m_colorDescList.size(); i++)
	{
		m_colorDescList.at(i).length = m_str.size() - m_colorDescList.at(m_colorDescCount).beginIndex;
	}
	for (size_t i = m_rubyDescCount; i < m_rubyDescList.size(); i++)
	{
		m_rubyDescList.at(i).length = m_str.size() - m_rubyDescList.at(m_rubyDescCount).beginIndex;
	}
}

// タグを削除
void Renderings::DecorationString::DeleteTag(std::wstring* pStr)
{
	while (true)
	{
		// < を探す
		size_t ltIndex = pStr->find(L"<");
		// 見つからなければ終了
		if (ltIndex == std::wstring::npos)
		{
			return;
		}

		// > を探す
		size_t gtIndex = pStr->find(L">", ltIndex);
		// 見つからなければ終了
		if (gtIndex == std::wstring::npos)
		{
			*pStr = pStr->substr(0, ltIndex);
			return;
		}

		pStr->erase(ltIndex, gtIndex - ltIndex);
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
		// ルビの設定
		else if (tag.substr(0, 5) == L"ruby=")
		{
			m_rubyDescList.push_back(RubyDesc{ tag.substr(5), m_str.size() });
		}
		// ルビの設定の終了
		else if (tag == L"/ruby")
		{
			m_rubyDescList.at(m_rubyDescCount).length = m_str.size() - m_rubyDescList.at(m_rubyDescCount).beginIndex;
			m_rubyDescCount++;
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
