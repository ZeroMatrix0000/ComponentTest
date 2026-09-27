/*
 * FileName:     TextRenderer.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/09/28
 *
 * テキスト描画
 */

#include "Pch.h"
#include "TextRenderer.h"

#include "TextOutlineRenderer.h"
#include "Text.h"
#include "Canvas.h"
#include "PixelShader.h"
#include "../GameObjects/GameObject.h"
#include "../Components/RectTransform.h"
#include "../Systems/IErrorMessage.h"

// コンストラクタ
Renderings::TextRenderer::TextRenderer()
	: ITextRenderer{}
	, m_dWriteFactory{}
	, m_textBitmap{}
	, m_fontCollection{}
	, m_strokeStyle{}
	, m_pTexts{}
	, m_pContext{}
	, m_pBackBuffer{}
{
}

// 初期化処理
void Renderings::TextRenderer::Initialize(IDXGISwapChain4* pSwapChain, ID2D1DeviceContext7* pContext, ID2D1Bitmap1* pBackBuffer)
{
	m_pContext = pContext;
	m_pBackBuffer = pBackBuffer;

	// DirectWrite
	if (m_dWriteFactory.Get() == nullptr)
	{
		Utility::ThrowIfFailed(DWriteCreateFactory
		(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(IDWriteFactory8),
			reinterpret_cast<IUnknown**>(m_dWriteFactory.GetAddressOf())
		));
	}

	// 線のスタイルの詳細（角を丸める）
	//D2D1_STROKE_STYLE_PROPERTIES strokeStyleProps{};
	//strokeStyleProps.lineJoin = D2D1_LINE_JOIN_ROUND;
	//m_d2DFactory.Get()->CreateStrokeStyle(
	//	strokeStyleProps,
	//	nullptr,
	//	0,
	//	m_strokeStyle.ReleaseAndGetAddressOf()
	//);

	// ビットマップの詳細
	D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(
		D2D1_BITMAP_OPTIONS_TARGET,
		D2D1::PixelFormat(
			DXGI_FORMAT_R8G8B8A8_UNORM,
			D2D1_ALPHA_MODE_PREMULTIPLIED
		)
	);

	Utility::ThrowIfFailed(m_pContext->CreateBitmap
	(
		m_pBackBuffer->GetPixelSize(),
		nullptr,
		0,
		bitmapProperties,
		m_textBitmap.ReleaseAndGetAddressOf()
	));
}

// フォントコレクションの作成
void Renderings::TextRenderer::CreateFontCollection(const std::wstring& directoryPath)
{
	// パスが存在しないなら
	if (!std::filesystem::exists(directoryPath))
	{
		// エラーメッセージを追加
		Systems::IErrorMessage::GetInstance()->AddMessage(Utility::FormatWString
		(
			L"パスが間違っています。: %s",
			directoryPath.c_str()
		));
		return;
	}

	// フォントファイル
	std::vector<Microsoft::WRL::ComPtr<IDWriteFontFile>> fontFiles;

	// ディレクトリ内を全て検索
	for (const auto& entry : std::filesystem::recursive_directory_iterator(directoryPath))
	{
		// ファイルなら
		if (entry.is_regular_file())
		{
			fontFiles.push_back(Microsoft::WRL::ComPtr<IDWriteFontFile>{});
			if (FAILED(m_dWriteFactory->CreateFontFileReference(entry.path().c_str(), nullptr, &fontFiles.back())))
			{
				fontFiles.pop_back();
				// エラーメッセージを追加
				Systems::IErrorMessage::GetInstance()->AddMessage(Utility::FormatWString
				(
					L"フォントの読み込みに失敗しました。: %s",
					entry.path().c_str()
				));
			}
		}
	}

	// ビルダー
	Microsoft::WRL::ComPtr<IDWriteFontSetBuilder2> builder;
	Utility::ThrowIfFailed(m_dWriteFactory->CreateFontSetBuilder(builder.GetAddressOf()));

	for (const auto& fontFile : fontFiles)
	{
		Utility::ThrowIfFailed(builder->AddFontFile(fontFile.Get()));
	}

	// フォントセット
	Microsoft::WRL::ComPtr<IDWriteFontSet> fontSet;
	Utility::ThrowIfFailed(builder->CreateFontSet(fontSet.GetAddressOf()));

	// フォントコレクション
	Microsoft::WRL::ComPtr<IDWriteFontCollection1> fontCollection;
	Utility::ThrowIfFailed(m_dWriteFactory->CreateFontCollectionFromFontSet
	(
		fontSet.Get(),
		fontCollection.GetAddressOf()
	));
	Utility::ThrowIfFailed(fontCollection.As(&m_fontCollection));
}

// 描画開始
void Renderings::TextRenderer::Begin()
{
	m_pContext->BeginDraw();
}

// 描画処理
void Renderings::TextRenderer::Draw(const Text* pText)
{
	// 空文字列または透明なら何もしない
	if (pText->GetStr().empty())
	{
		return;
	}

	// テキストの所有者の2Dトランスフォーム
	const RectTransform* pRectTransform = pText->GetConstPOwner()->GetConstComponent<RectTransform>();

	// テキストが映るキャンバス
	const Canvas* pCanvas = pText->GetPCanvas();
	if (!pCanvas)
	{
		return;
	}

	// 長方形
	Math::Rect rect = pRectTransform->GetRect();
	// キャンバスサイズ
	Math::Vector2 canvasSize = pCanvas->GetSize();

	// ピボットに合わせて長方形を移動
	switch (pRectTransform->GetPivot())
	{
	case Utility::AlignmentPoint::TopLeft:
		rect.position.x += rect.size.x / 2.0f;
		rect.position.y += rect.size.y / 2.0f;
		break;
	case Utility::AlignmentPoint::TopCenter:
		rect.position.y += rect.size.y / 2.0f;
		break;
	case Utility::AlignmentPoint::TopRight:
		rect.position.x -= rect.size.x / 2.0f;
		rect.position.y += rect.size.y / 2.0f;
		break;
	case Utility::AlignmentPoint::MiddleLeft:
		rect.position.x += rect.size.x / 2.0f;
		break;
	case Utility::AlignmentPoint::MiddleCenter:
		break;
	case Utility::AlignmentPoint::MiddleRight:
		rect.position.x -= rect.size.x / 2.0f;
		break;
	case Utility::AlignmentPoint::BottomLeft:
		rect.position.x += rect.size.x / 2.0f;
		rect.position.y -= rect.size.y / 2.0f;
		break;
	case Utility::AlignmentPoint::BottomCenter:
		rect.position.y -= rect.size.y / 2.0f;
		break;
	case Utility::AlignmentPoint::BottomRight:
		rect.position.x -= rect.size.x / 2.0f;
		rect.position.y -= rect.size.y / 2.0f;
		break;
	default:
		break;
	}
	// アンカーに合わせて長方形を移動
	switch (pRectTransform->GetAnchor())
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
	float canvasRatio = pCanvas->GetRatio();
	// 表示倍率を適用
	rect.position *= canvasRatio;
	rect.size *= canvasRatio;

	// テキストフォーマット
	Microsoft::WRL::ComPtr<IDWriteTextFormat> textFormat;
	if (FAILED(m_dWriteFactory->CreateTextFormat
	(
		pText->GetFontName().c_str(),
		m_fontCollection.Get(),
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		pText->GetFontSize() * canvasRatio,
		L"ja-jp",
		textFormat.GetAddressOf()
	)))
	{
		return;
	}

	// 左右配置
	textFormat->SetTextAlignment(pText->GetTextAlignment());
	// 上下配置
	textFormat->SetParagraphAlignment(pText->GetParagraphAlignment());

	// 描画する文字列
	const std::wstring& str = pText->GetStr();

	// テキストレイアウト
	Microsoft::WRL::ComPtr<IDWriteTextLayout> textLayout;
	m_dWriteFactory->CreateTextLayout
	(
		str.c_str(),
		static_cast<UINT32>(str.size()),
		textFormat.Get(),
		rect.size.x,
		rect.size.y,
		textLayout.GetAddressOf()
	);

	// テキストブラシ
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> textBrush;
	m_pContext->CreateSolidColorBrush
	(
		pText->GetD2D1FontColor(),
		textBrush.GetAddressOf()
	);

	if (pText->GetOutlineWidth() != 0.0f)
	{
		// 描画終了
		m_pContext->EndDraw();

		// 描画先を設定
		m_pContext->SetTarget(m_textBitmap.Get());
		// 描画開始
		m_pContext->BeginDraw();
		// 透明に
		m_pContext->Clear();
	}


	// 角度
	float angle = pRectTransform->GetAngle();
	// 描画ターゲットを回転
	if (angle != 0.0f)
	{
		m_pContext->SetTransform(D2D1::Matrix3x2F::Rotation
		(
			angle,
			D2D1::Point2F(rect.position.x, rect.position.y)
		));
	}

	//// 文字列を描画
	//TextOutlineRenderer outlineRenderer
	//{
	//	canvasRatio,
	//	m_d2DFactory.Get(),
	//	m_renderTarget.Get(),
	//	textBrush.Get(),
	//	outlineBrush.Get(),
	//	m_strokeStyle.Get(),
	//	*pText
	//};
	//outlineRenderer.Begin();
	//textLayout->Draw(nullptr, &outlineRenderer, rect.position.x - rect.size.x / 2.0f, rect.position.y - rect.size.y / 2.0f);
	//outlineRenderer.End();

	// 文字を描画
	m_pContext->DrawTextLayout
	(
		D2D1::Point2F(rect.position.x - rect.size.x / 2.0f, rect.position.y - rect.size.y / 2.0f),
		textLayout.Get(),
		textBrush.Get()
	);

	// 描画ターゲットを元に戻す
	if (angle != 0.0f)
	{
		m_pContext->SetTransform(D2D1::Matrix3x2F::Identity());
	}
	m_pContext->SetTransform(D2D1::Matrix3x2F::Identity());



	if (pText->GetOutlineWidth() != 0.0f)
	{
		// 描画終了
		m_pContext->EndDraw();

		// 描画先を戻す
		m_pContext->SetTarget(m_pBackBuffer);
		// 描画開始
		m_pContext->BeginDraw();

		// 膨張エフェクト
		Microsoft::WRL::ComPtr<ID2D1Effect> dilate;
		m_pContext->CreateEffect(CLSID_D2D1Morphology, dilate.GetAddressOf());
		dilate->SetInput(0, m_textBitmap.Get());
		dilate->SetValue(D2D1_MORPHOLOGY_PROP_MODE, D2D1_MORPHOLOGY_MODE_DILATE);
		dilate->SetValue(D2D1_MORPHOLOGY_PROP_WIDTH, 1 + Math::RoundInt(pText->GetOutlineWidth() * canvasRatio * 2.0f));
		dilate->SetValue(D2D1_MORPHOLOGY_PROP_HEIGHT, 1 + Math::RoundInt(pText->GetOutlineWidth() * canvasRatio * 2.0f));
		Microsoft::WRL::ComPtr<ID2D1Image> dilateImage;
		dilate->GetOutput(dilateImage.GetAddressOf());
		// 色変更エフェクト
		Microsoft::WRL::ComPtr<ID2D1Effect> flood;
		m_pContext->CreateEffect(CLSID_D2D1Flood, flood.GetAddressOf());
		flood->SetValue(D2D1_FLOOD_PROP_COLOR, pText->GetD2D1OutlineColor());
		Microsoft::WRL::ComPtr<ID2D1Image> floodImage;
		flood->GetOutput(floodImage.GetAddressOf());
		// エフェクト合体
		Microsoft::WRL::ComPtr<ID2D1Effect> composite;
		m_pContext->CreateEffect(CLSID_D2D1Composite, composite.GetAddressOf());
		composite->SetInput(0, dilateImage.Get());
		composite->SetInput(1, floodImage.Get());
		composite->SetValue(D2D1_COMPOSITE_PROP_MODE, D2D1_COMPOSITE_MODE_SOURCE_IN);

		// 文字を描画
		m_pContext->DrawImage(composite.Get(), D2D1::Point2F());
		m_pContext->DrawBitmap(m_textBitmap.Get());
	}
}

// 描画終了
void Renderings::TextRenderer::End()
{
	m_pContext->EndDraw();
}

// テキストのポインタを追加
void Renderings::TextRenderer::AddPText(const Text* pText)
{
	m_pTexts.push_back(pText);
}

// テキストのポインタを削除
void Renderings::TextRenderer::RemovePText(const Text* pText)
{
	auto it = std::ranges::find(m_pTexts, pText);
	if (it != m_pTexts.end())
	{
		m_pTexts.erase(it);
	}
}

// テキストのポインタリストをソート
void Renderings::TextRenderer::SortPTexts()
{
	// レイヤー順にソート
	std::ranges::sort(m_pTexts, [](const Text* p1, const Text* p2) {return p1->GetOrderInLayer() < p2->GetOrderInLayer(); });
}
