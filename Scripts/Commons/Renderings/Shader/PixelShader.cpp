/*
 * FileName:     PixelShader.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/07
 *
 * ピクセルシェーダ
 */

#include "Pch.h"
#include "PixelShader.h"

// コンストラクタ
Renderings::PixelShader::PixelShader()
	: m_d3dShader{}
	, m_constantBuffer{}
{
}

// 初期化処理
void Renderings::PixelShader::Initialize(ID3D11Device5* pDevice, const std::wstring& filePath)
{
	// ブロブデータ
	Utility::ThrowIfFailed(D3DReadFileToBlob(filePath.c_str(), m_blob.GetAddressOf()));

	// ピクセルシェーダ
	Utility::ThrowIfFailed(pDevice->CreatePixelShader
	(
		m_blob->GetBufferPointer(),
		m_blob->GetBufferSize(),
		nullptr,
		m_d3dShader.GetAddressOf()
	));

	// 定数バッファ
	m_constantBuffer = std::make_unique<ConstantBuffer>();
	m_constantBuffer->Initialize(pDevice, m_blob.Get());
}

// 定数バッファを GPU に送信
void Renderings::PixelShader::SetConstantBuffer(ID3D11Device5* pDevice, ID3D11DeviceContext4* pContext) const
{
	auto& data = GetConstantBuffer()->GetData();
	auto* buffer = GetConstantBuffer()->GetBuffer();

	if (data.size() != 0)
	{
		// 定数バッファを設定
		pContext->UpdateSubresource(buffer, 0, nullptr, data.data(), 0, 0);
		pContext->PSSetConstantBuffers(0, 1, &buffer);
	}
}

// 生成
Renderings::PixelShader Renderings::PixelShader::Create(ID3D11Device5* pDevice, const std::wstring& filePath)
{
	PixelShader shader;
	shader.Initialize(pDevice, filePath);
	return shader;
}
