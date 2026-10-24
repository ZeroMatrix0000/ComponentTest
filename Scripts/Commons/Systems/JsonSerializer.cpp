/*
 * FileName:     JsonSerializer.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/22
 *
 * Jsonのシリアライズ
 */

#include "Pch.h"
#include "JsonSerializer.h"

#include "../Systems/IErrorMessage.h"

std::unordered_map<std::type_index, std::function<void(void*, const nlohmann::ordered_json&, Ref<IGameObjectFinder>)>> Systems::JsonSerializer::s_methods{};

// コンストラクタ
Systems::JsonSerializer::JsonSerializer(Ref<IGameObjectFinder> iGameObjectFinderRef)
	: m_parameters{}
	, m_refIGameObjectFinder{ iGameObjectFinderRef }
{
}

// パラメータを読み込む
void Systems::JsonSerializer::Load(const nlohmann::ordered_json& json)
{
	for (const auto& element : json.items())
	{
		// パラメータ名
		const std::string& name = element.key();
		
		// パラメータを探す
		auto it = m_parameters.find(name);
		// パラメータが見つからなかったら
		if (it == m_parameters.end())
		{
			// エラーメッセージを追加
			IErrorMessage::GetInstance()->AddMessage(Utility::FormatWString
			(
				L"パラメータが見つかりません。 | name: %s",
				Utility::string2wstring(name.c_str()).c_str()
			));
			continue;
		}

		try
		{
			s_methods.at(it->second.index)(it->second.ptr, element.value(), m_refIGameObjectFinder);
		}
		catch (std::exception e)
		{
			// エラーメッセージを追加
			IErrorMessage::GetInstance()->AddMessage(Utility::FormatWString
			(
				L"パラメータの読み込みに失敗しました。 | name: %s",
				Utility::string2wstring(name.c_str()).c_str()
			));
			continue;
		}
	}
}
