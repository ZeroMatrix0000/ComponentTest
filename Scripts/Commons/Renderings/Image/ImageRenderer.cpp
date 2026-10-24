/*
 * FileName:     ImageRenderer.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/24
 *
 * 画像描画
 */

#include "Pch.h"
#include "ImageRenderer.h"

#include "ImageSource.h"
#include "../Shader/PixelShader.h"
#include "Image.h"
#include "../Canvas.h"
#include "Scripts/Commons/Systems/IResources.h"
#include "Scripts/Commons/GameObjects/GameObject.h"
#include "Scripts/Commons/Components/RectTransform.h"
#include "Scripts/Commons/Systems/IErrorMessage.h"
#include "Scripts/Commons/Systems/IsDerivedRegister.h"

// コンストラクタ
Renderings::ImageRenderer::ImageRenderer()
	: IImageRenderer{}
	, m_spriteBatch{}
	, m_imageRefs{}
	, m_pDevice{}
	, m_pContext{}
	, m_pCommonStates{}
{
}

// 初期化処理
void Renderings::ImageRenderer::Initialize
(
	Ref<ID3D11Device5> deviceRef,
	Ref<ID3D11DeviceContext4> contextRef,
	Ref<const DirectX::CommonStates> commonStatesRef
)
{
	m_pDevice = &deviceRef.Get();
	m_pContext = &contextRef.Get();
	m_pCommonStates = &commonStatesRef.Get();
	m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(m_pContext);
}

// 描画開始
void Renderings::ImageRenderer::Begin()
{
	// 描画開始
	m_spriteBatch->Begin
	(
		DirectX::SpriteSortMode_Immediate,
		m_pCommonStates->NonPremultiplied()
	);
}

// 描画処理
void Renderings::ImageRenderer::Draw(Ref<const Image> imageRef)
{
	// 所有者
	Ref owner = imageRef->GetConstOwner();
	if (owner == imageRef->GetNullGameObject().Const())
	{
		return;
	}

	// 画像ソース
	Ref<const ImageSource> imageSource = imageRef->GetImageSource();
	if (!imageSource->IsInitialized())
	{
		// エラーメッセージを追加
		Systems::IErrorMessage::GetInstance()->AddMessage(Utility::FormatWString
		(
			L"ImageSource が設定されていません。 | %s",
			Utility::string2wstring(imageRef->GetConstOwner()->GetName()).c_str()
		));
		return;
	}

	// 画像の所有者の2Dトランスフォーム
	Ref rectTransform{ imageRef->GetConstOwner()->GetConstComponent<RectTransform>() };
	if (rectTransform == imageRef->GetConstOwner()->GetNullComponent<RectTransform>().Const())
	{
		// エラーメッセージを追加
		Systems::IErrorMessage::GetInstance()->AddMessage(Utility::FormatWString
		(
			L"RectTransform コンポーネントが見つかりません。 | %s",
			Utility::string2wstring(imageRef->GetConstOwner()->GetName()).c_str()
		));
		return;
	}

	// 画像が映るキャンバス
	Ref canvasRef = imageRef->GetCanvasRef();
	if (canvasRef == imageRef->GetConstOwner()->GetNullComponent<Renderings::Canvas>().Const())
	{
		// エラーメッセージを追加
		Systems::IErrorMessage::GetInstance()->AddMessage(Utility::FormatWString
		(
			L"Canvas が設定されていません。 | %s",
			Utility::string2wstring(imageRef->GetConstOwner()->GetName()).c_str()
		));
		return;
	}

	// テクスチャの詳細
	D3D11_TEXTURE2D_DESC textureDesc;
	imageSource->GetResource()->GetDesc(&textureDesc);

	// テクスチャの中心
	Math::Vector2 origin = Math::Vector2
	{
		static_cast<float>(textureDesc.Width),
		static_cast<float>(textureDesc.Height)
	};

	// ピボットにあわせて画像の中心を移動
	switch (rectTransform->GetPivot())
	{
	case Utility::AlignmentPoint::TopLeft:
		origin.x = 0.0f;
		origin.y = 0.0f;
		break;
	case Utility::AlignmentPoint::TopCenter:
		origin.x /= 2.0f;
		origin.y = 0.0f;
		break;
	case Utility::AlignmentPoint::TopRight:
		origin.y = 0.0f;
		break;
	case Utility::AlignmentPoint::MiddleLeft:
		origin.x = 0.0f;
		origin.y /= 2.0f;
		break;
	case Utility::AlignmentPoint::MiddleCenter:
		origin.x /= 2.0f;
		origin.y /= 2.0f;
		break;
	case Utility::AlignmentPoint::MiddleRight:
		origin.y /= 2.0f;
		break;
	case Utility::AlignmentPoint::BottomLeft:
		origin.x = 0.0f;
		break;
	case Utility::AlignmentPoint::BottomCenter:
		origin.x /= 2.0f;
		break;
	default:
		break;
	}

	// 長方形
	Math::Rect rect = rectTransform->GetRect();
	// キャンバスサイズ
	Math::Vector2 canvasSize = canvasRef->GetSize();

	// アンカーに合わせて画像を移動
	switch (rectTransform->GetAnchor())
	{
	case Utility::AlignmentPoint::TopCenter:
		rect.position.x += canvasSize.x / 2.0f;
		break;
	case Utility::AlignmentPoint::TopRight:
		rect.position.x += canvasSize.x;
		break;
	case Utility::AlignmentPoint::MiddleLeft:
		rect.position.y += canvasSize.y / 2.0f;
		break;
	case Utility::AlignmentPoint::MiddleCenter:
		rect.position.x += canvasSize.x / 2.0f;
		rect.position.y += canvasSize.y / 2.0f;
		break;
	case Utility::AlignmentPoint::MiddleRight:
		rect.position.x += canvasSize.x;
		rect.position.y += canvasSize.y / 2.0f;
		break;
	case Utility::AlignmentPoint::BottomLeft:
		rect.position.y += canvasSize.y;
		break;
	case Utility::AlignmentPoint::BottomCenter:
		rect.position.x += canvasSize.x / 2.0f;
		rect.position.y += canvasSize.y;
		break;
	case Utility::AlignmentPoint::BottomRight:
		rect.position.x += canvasSize.x;
		rect.position.y += canvasSize.y;
		break;
	default:
		break;
	}

	// キャンバスの表示倍率
	float canvasRatio = canvasRef->GetRatio();
	// 表示倍率を適用
	rect.position *= canvasRatio;
	rect.size *= canvasRatio;

	// ピクセルシェーダ
	Ref<const Renderings::PixelShader> pixelShader = imageRef->GetPixelShader();

	if (pixelShader->IsInitialized())
	{
		// ピクセルシェーダを設定
		m_pContext->PSSetShader(pixelShader->GetD3DShader(), nullptr, 0);
		// 定数バッファを設定
		pixelShader->SetConstantBuffer(m_pDevice, m_pContext, 0);
	}

	// 描画
	m_spriteBatch->Draw
	(
		imageSource->GetTexture(),
		RECT
		{
			Math::RoundInt(rect.position.x),
			Math::RoundInt(rect.position.y),
			Math::RoundInt(rect.position.x + rect.size.x),
			Math::RoundInt(rect.position.y + rect.size.y),
		},
		nullptr,
		imageRef->GetColor(),
		Math::Deg2Rad(rectTransform->GetAngle()),
		origin
		);

	if (pixelShader->IsInitialized())
	{
		m_pContext->PSSetShader(nullptr, nullptr, 0);
	}
}

// 描画終了
void Renderings::ImageRenderer::End()
{
	// 描画終了
	m_spriteBatch->End();
}

// 画像のポインタを追加
void Renderings::ImageRenderer::AddImageRef(Ref<const Image> imageRef)
{
	m_imageRefs.push_back(imageRef);
}

// 画像のポインタを削除
void Renderings::ImageRenderer::RemoveImageRef(Ref<const Image> imageRef)
{
	auto it = std::ranges::find(m_imageRefs, imageRef);
	if (it != m_imageRefs.end())
	{
		m_imageRefs.erase(it);
	}
}

// 画像の大きさを取得
Math::Vector2 Renderings::ImageRenderer::GetImageSize(Ref<const Image> imageRef) const
{
	// 画像ソース
	Ref<const ImageSource> imageSource = imageRef->GetImageSource();
	if (!imageSource->IsInitialized())
	{
		return Math::Vector2::Zero;
	}

	// テクスチャの詳細
	D3D11_TEXTURE2D_DESC textureDesc;
	imageSource->GetResource()->GetDesc(&textureDesc);

	return Math::Vector2
	{
		static_cast<float>(textureDesc.Width),
		static_cast<float>(textureDesc.Height)
	};
}

// 画像のポインタリストのソート
void Renderings::ImageRenderer::SortPImages()
{
	// レイヤー順にソート
	std::ranges::sort(m_imageRefs, [](Ref<const Image> ref1, Ref<const Image> ref2) {return ref1->GetOrderInLayer() < ref2->GetOrderInLayer(); });
}
