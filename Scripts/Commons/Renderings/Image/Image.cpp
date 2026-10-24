/*
 * FileName:     Image.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/24
 *
 * 画像
 */

#include "Pch.h"
#include "Image.h"

#include "IImageRenderer.h"
#include "Scripts/Commons/GameObjects/GameObject.h"
#include "Scripts/Commons/Systems/IResources.h"
#include "Scripts/Commons/Systems/JsonSerializer.h"
#include "Scripts/Commons/Systems/IsDerivedRegister.h"

// コンストラクタ
Renderings::Image::Image(const ComponentDesc& desc, Ref<IImageRenderer> iImageRendererRef, Ref<Systems::IResources> iResources)
	: Component{ desc }
	, m_color{ DirectX::Colors::White }
	, m_orderInLayer{}
	, m_imageSource{ iResources->GetNullImageSource() }
	, m_pixelShader{ iResources->GetNullPixelShader() }
	, m_canvasRef{ GetOwner()->GetNullComponent<Renderings::Canvas>().Const() }
	, m_iImageRendererRef{ iImageRendererRef }
	, m_iResourcesRef{ iResources }
{
	// 画像描画に参照を追加
	m_iImageRendererRef->AddImageRef(Ref{ *this }.Const());
}

// デストラクタ
Renderings::Image::~Image()
{
	// 画像描画から参照を削除
	m_iImageRendererRef->RemoveImageRef(Ref{ *this }.Const());
}

// 初期化処理
void Renderings::Image::Initalize(const nlohmann::ordered_json& json, Ref<IGameObjectFinder> iGameObjectFinderRef)
{
	std::string imageSourceName{};
	std::string pixelShaderName{};

	Systems::JsonSerializer serializer{ iGameObjectFinderRef };
	serializer.AddParameter(&imageSourceName, "ImageSourceName");
	serializer.AddParameter(&m_color, "Color");
	serializer.AddParameter(&pixelShaderName, "PixelShaderName");
	serializer.AddParameter(&m_orderInLayer, "OrderInLayer");
	serializer.AddParameter(&m_canvasRef, "Canvas");
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
	m_imageSource = m_iResourcesRef->GetImageSource(imageSourceName);
}

// ピクセルシェーダを設定
void Renderings::Image::SetPixelShader(const std::string& pixelShaderName)
{
	m_pixelShader = m_iResourcesRef->GetPixelShader(pixelShaderName);
}

// 画像サイズを取得
Math::Vector2 Renderings::Image::GetSize() const
{
	return m_iImageRendererRef->GetImageSize(Ref{ *this });
}
