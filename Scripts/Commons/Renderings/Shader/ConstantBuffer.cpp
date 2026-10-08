/*
 * FileName:     ConstantBuffer.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/08
 *
 * 定数バッファ
 */

#include "Pch.h"
#include "ConstantBuffer.h"

// コンストラクタ
Renderings::ConstantBuffer::ConstantBuffer()
	: m_data{}
	, m_variables{}
	, m_buffer{}
{
}

// 初期化処理
void Renderings::ConstantBuffer::Initialize(ID3D11Device5* pDevice, ID3DBlob* pBlob)
{
	// シェーダ情報
	Microsoft::WRL::ComPtr<ID3D11ShaderReflection> reflection{};
	Utility::ThrowIfFailed(D3DReflect
	(
		pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(),
		IID_PPV_ARGS(reflection.GetAddressOf())
	));

	// シェーダ詳細
	D3D11_SHADER_DESC shaderDesc{};
	Utility::ThrowIfFailed(reflection->GetDesc(&shaderDesc));

	for (UINT i = 0; i < shaderDesc.ConstantBuffers; i++)
	{
		// 定数バッファ
		ID3D11ShaderReflectionConstantBuffer* buffer = reflection->GetConstantBufferByIndex(i);

		// バッファ詳細
		D3D11_SHADER_BUFFER_DESC bufferDesc{};
		Utility::ThrowIfFailed(buffer->GetDesc(&bufferDesc));

		m_data.resize(m_data.size() + bufferDesc.Size);

		for (UINT j = 0; j < bufferDesc.Variables; j++)
		{
			// 変数
			ID3D11ShaderReflectionVariable* variable = buffer->GetVariableByIndex(j);

			// 変数バッファ
			D3D11_SHADER_VARIABLE_DESC variableDesc{};
			Utility::ThrowIfFailed(variable->GetDesc(&variableDesc));

			// 変数リストに追加
			m_variables.emplace
			(
				variableDesc.Name,
				Variable{ variableDesc.StartOffset, variableDesc.Size }
			);
		}
	}

	if (m_data.size() != 0)
	{
		// 定数バッファの詳細
		CD3D11_BUFFER_DESC cbDesc{ static_cast<UINT>(m_data.size()), D3D11_BIND_CONSTANT_BUFFER };
		// d3dバッファ
		Utility::ThrowIfFailed(pDevice->CreateBuffer(&cbDesc, nullptr, m_buffer.GetAddressOf()));
	}
}
