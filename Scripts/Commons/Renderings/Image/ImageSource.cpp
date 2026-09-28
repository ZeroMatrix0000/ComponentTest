/*
 * FileName:     ImageSource.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/07/06
 *
 * 画像ソース
 */

#include "Pch.h"
#include "ImageSource.h"

 // コンストラクタ
Renderings::ImageSource::ImageSource()
	: m_texture{}
	, m_resource{}
{
}

// 初期化処理
void Renderings::ImageSource::Initialize(ID3D11Device5* device, const std::wstring& filePath)
{
	// テクスチャ
	Microsoft::WRL::ComPtr<ID3D11Resource> texture;

	Utility::ThrowIfFailed(DirectX::CreateDDSTextureFromFile
	(
		device,
		filePath.c_str(),
		texture.GetAddressOf(),
		m_texture.ReleaseAndGetAddressOf()
	));
	Utility::ThrowIfFailed(texture.As(&m_resource));
}

// 生成
Renderings::ImageSource Renderings::ImageSource::Create(ID3D11Device5* device, const std::wstring& filePath)
{
	ImageSource image;
	image.Initialize(device, filePath);
	return image;
}
