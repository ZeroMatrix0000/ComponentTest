/*
 * FileName:     ConstantBuffer.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/08
 *
 * 定数バッファ
 */

#pragma once

#include "Scripts/Commons/Systems/IErrorMessage.h"

namespace Renderings
{
	class ConstantBuffer
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		ConstantBuffer();

		// 初期化処理
		void Initialize(ID3D11Device5* pDevice, ID3DBlob* pBlob);

		// 値を変更
		template<typename T>
		void SetVariable(const std::string& name, T value)
		{
			auto it = m_variables.find(name);
			if (it == m_variables.end() || it->second.size != sizeof(T))
			{
				// エラーメッセージを追加
				Systems::IErrorMessage::GetInstance()->AddMessage(Utility::FormatWString
				(
					L"変数が見つかりません。 | name: %s",
					Utility::string2wstring(name).c_str()
				));
				return;
			}

			std::memcpy(m_data.data() + it->second.offset, &value, sizeof(T));
		}

		// 値を取得
		template<typename T>
		T GetVariable(const std::string& name) const
		{
			auto it = m_variables.find(name);
			if (it == m_variables.end() || it->second.size != sizeof(T))
			{
				// エラーメッセージを追加
				Systems::IErrorMessage::GetInstance()->AddMessage(Utility::FormatWString
				(
					L"変数が見つかりません。 | name: %s",
					Utility::string2wstring(name).c_str()
				));
				return T{};
			}

			T temp{};
			std::memcpy(&temp, m_data.data() + it->second.offset, sizeof(T));
			return temp;
		}

		// データを取得
		const std::vector<std::byte>& GetData() const { return m_data; }

		// d3dバッファを取得
		auto* GetBuffer() const { return m_buffer.Get(); }


	private:


		/* 構造体 */

		// 変数情報
		struct Variable
		{
			// 位置
			size_t offset;
			size_t size;
		};


		/* メンバ変数 */

		// データ
		std::vector<std::byte> m_data;

		// 変数リスト
		std::unordered_map<std::string, Variable> m_variables;

		// バッファ
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;

	};
}