/*
 * FileName:     Utility.h
 * Author:       Takao Hayata
 * Last Updated: 2026/07/14
 *
 * 便利関数群
 */

#pragma once

namespace Libraries
{
	// 便利関数群
	namespace Utility
	{
		// 位置合わせ点
		enum class AlignmentPoint
		{
			TopLeft,
			TopCenter,
			TopRight,
			MiddleLeft,
			MiddleCenter,
			MiddleRight,
			BottomLeft,
			BottomCenter,
			BottomRight,
		};

		// 例外処理
		void Throw();
		// 条件を満たしたら例外処理
		void ThrowIf(bool value);
		// 失敗したら例外処理
		void ThrowIfFailed(HRESULT hr);

		// string -> wstring
		std::wstring string2wstring(const std::string& value);
		// wstring -> string
		std::string  wstring2string(const std::wstring& value);

		// wstring -> float
		float wstring2float(const std::wstring& value);
		// wstring -> float
		std::wstring float2wstring(float value);

		// 文字列フォーマット
		template<typename... Args>
		std::string FormatString(const std::string& formatString, Args ...args)
		{
			int size = std::snprintf(nullptr, 0, formatString.c_str(), args ...) + 1;	// 文字列の長さ
			std::string str(size, 0);
			std::snprintf(&str[0], size, formatString.c_str(), args ...);	// 変換
			return str;
		}
		// 文字列フォーマット
		template<typename... Args>
		std::wstring FormatWString(const std::wstring& formatString, Args ...args)
		{
			int size = std::swprintf(nullptr, 0, formatString.c_str(), args ...) + 1;	// 文字列の長さ
			std::wstring wstr(size, 0);
			std::swprintf(&wstr[0], size, formatString.c_str(), args ...);	// 変換
			return wstr;
		}
	}
}
