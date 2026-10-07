/*
 * FileName:     VertexShader.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/07
 *
 * 頂点シェーダ
 */

#include "Pch.h"
#include "VertexShader.h"

 // コンストラクタ
Renderings::VertexShader::VertexShader()
	: m_d3dShader{}
	, m_constantBuffer{}
{
}

// 初期化処理
void Renderings::VertexShader::Initialize(ID3D11Device5* device, const std::wstring& filePath)
{
	// ブロブデータ
	Utility::ThrowIfFailed(D3DReadFileToBlob(filePath.c_str(), m_blob.GetAddressOf()));

	// ピクセルシェーダ
	Utility::ThrowIfFailed(device->CreateVertexShader
	(
		m_blob->GetBufferPointer(),
		m_blob->GetBufferSize(),
		nullptr,
		m_d3dShader.GetAddressOf()
	));

	// 定数バッファ
	m_constantBuffer = std::make_unique<ConstantBuffer>();
	m_constantBuffer->Initialize(m_blob.Get());
}

// 生成
Renderings::VertexShader Renderings::VertexShader::Create(ID3D11Device5* device, const std::wstring& filePath)
{
	VertexShader shader;
	shader.Initialize(device, filePath);
	return shader;
}
