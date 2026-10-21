/*
 * FileName:     Image.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/21
 *
 * 画像
 */

#include "Pch.h"
#include "Image.h"

#include "IImageRenderer.h"
#include "Scripts/Commons/GameObjects/IGameObjectFinder.h"
#include "Scripts/Commons/GameObjects/GameObject.h"
#include "Scripts/Commons/Renderings/Canvas.h"
#include "Scripts/Commons/Systems/IResources.h"
#include "Scripts/Commons/Systems/JsonSerializer.h"

// コンストラクタ
Renderings::Image::Image(const ComponentDesc& desc, IImageRenderer* pIImageRenderer, Systems::IResources* pIResources)
	: Component{ desc }
	, m_color{ DirectX::Colors::White }
	, m_orderInLayer{}
	, m_pImageSource{ pIResources->GetNullImageSource() }
	, m_pPixelShader{ pIResources->GetNullPixelShader() }
	, m_pCanvas{}
	, m_pIImageRenderer{ pIImageRenderer }
	, m_pIResources{ pIResources }
{
	// 画像描画にポインタを追加
	m_pIImageRenderer->AddPImage(this);
}

// デストラクタ
Renderings::Image::~Image()
{
	// 画像描画からポインタを削除
	m_pIImageRenderer->RemovePImage(this);
}

// 初期化処理
void Renderings::Image::Initalize(const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder)
{
	std::string imageSourceName{};
	std::string pixelShaderName{};

	Systems::JsonSerializer serializer{ pIGameObjectFinder };
	serializer.AddParameter(&imageSourceName, "ImageSourceName");
	serializer.AddParameter(&m_color, "Color");
	serializer.AddParameter(&pixelShaderName, "PixelShaderName");
	serializer.AddParameter(&m_orderInLayer, "OrderInLayer");
	serializer.AddParameter(&m_pCanvas, "Canvas");
	serializer.Load(json);

	if (!imageSourceName.empty())
	{
		SetImageSource(imageSourceName);
	}
	if (!pixelShaderName.empty())
	{
		SetPixelShader(pixelShaderName);
	}
}

// 画像ソースを設定
void Renderings::Image::SetImageSource(const std::string& imageSourceName)
{
	m_pImageSource = m_pIResources->GetImageSource(imageSourceName);
}

// ピクセルシェーダを設定
void Renderings::Image::SetPixelShader(const std::string& pixelShaderName)
{
	m_pPixelShader = m_pIResources->GetPixelShader(pixelShaderName);
}

// 画像サイズを取得
Math::Vector2 Renderings::Image::GetSize() const
{
	return m_pIImageRenderer->GetImageSize(this);
}
