/*
 * FileName:     TextRenderer.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/09/30
 *
 * テキスト描画
 */

#include "Pch.h"
#include "TextRenderer.h"

#include "TextOutlineRenderer.h"
#include "Text.h"
#include "../Canvas.h"
#include "../PixelShader.h"
#include "Scripts/Commons/GameObjects/GameObject.h"
#include "Scripts/Commons/Components/RectTransform.h"
#include "Scripts/Commons/Systems/IErrorMessage.h"

// コンストラクタ
Renderings::TextRenderer::TextRenderer()
	: ITextRenderer{}
	, m_dWriteFactory{}
	, m_textContext{}
	, m_fontCollection{}
	, m_dilateEffect{}
	, m_floodEffect{}
	, m_compositeEffect{}
	, m_decorationRenderer{}
	, m_pTexts{}
	, m_pContext{}
	, m_pBackBuffer{}
{
}

// 初期化処理
void Renderings::TextRenderer::Initialize(ID2D1Device7* pDevice, ID2D1DeviceContext7* pContext, ID2D1Bitmap1* pBackBuffer)
{
	m_pContext = pContext;
	m_pBackBuffer = pBackBuffer;

	// DirectWrite
	Utility::ThrowIfFailed(DWriteCreateFactory
	(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory8),
		reinterpret_cast<IUnknown**>(m_dWriteFactory.GetAddressOf())
	));

	// コンテキスト
	Utility::ThrowIfFailed(pDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, m_textContext.GetAddressOf()));

	// 膨張エフェクト
	m_textContext->CreateEffect(CLSID_D2D1Morphology, m_dilateEffect.GetAddressOf());
	m_dilateEffect->SetValue(D2D1_MORPHOLOGY_PROP_MODE, D2D1_MORPHOLOGY_MODE_DILATE);
	// 色変更エフェクト
	m_textContext->CreateEffect(CLSID_D2D1Flood, m_floodEffect.GetAddressOf());
	// エフェクト合体
	m_textContext->CreateEffect(CLSID_D2D1Composite, m_compositeEffect.GetAddressOf());
	m_compositeEffect->SetValue(D2D1_COMPOSITE_PROP_MODE, D2D1_COMPOSITE_MODE_SOURCE_IN);

	OnWindowSizeChanged(pContext, pBackBuffer);
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
void Renderings::TextRenderer::Draw(const Text* pText, bool isDebug)
{
	// 空文字列なら何もしない
	if (pText->GetStr().empty())
	{
		return;
	}

	// テキストが透明なら何もしない
	if (pText->GetFontColor().A() == 0.0f)
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

	// トランスフォームの長方形
	Math::Rect transformRect = pRectTransform->GetRect();
	// キャンバスサイズ
	Math::Vector2 canvasSize = pCanvas->GetSize();

	// ピボットに合わせて長方形を移動
	switch (pRectTransform->GetPivot())
	{
	case Utility::AlignmentPoint::TopLeft:
		transformRect.position.x += transformRect.size.x / 2.0f;
		transformRect.position.y += transformRect.size.y / 2.0f;
		break;
	case Utility::AlignmentPoint::TopCenter:
		transformRect.position.y += transformRect.size.y / 2.0f;
		break;
	case Utility::AlignmentPoint::TopRight:
		transformRect.position.x -= transformRect.size.x / 2.0f;
		transformRect.position.y += transformRect.size.y / 2.0f;
		break;
	case Utility::AlignmentPoint::MiddleLeft:
		transformRect.position.x += transformRect.size.x / 2.0f;
		break;
	case Utility::AlignmentPoint::MiddleCenter:
		break;
	case Utility::AlignmentPoint::MiddleRight:
		transformRect.position.x -= transformRect.size.x / 2.0f;
		break;
	case Utility::AlignmentPoint::BottomLeft:
		transformRect.position.x += transformRect.size.x / 2.0f;
		transformRect.position.y -= transformRect.size.y / 2.0f;
		break;
	case Utility::AlignmentPoint::BottomCenter:
		transformRect.position.y -= transformRect.size.y / 2.0f;
		break;
	case Utility::AlignmentPoint::BottomRight:
		transformRect.position.x -= transformRect.size.x / 2.0f;
		transformRect.position.y -= transformRect.size.y / 2.0f;
		break;
	default:
		break;
	}
	// アンカーに合わせて長方形を移動
	switch (pRectTransform->GetAnchor())
	{
	case Utility::AlignmentPoint::TopCenter:
		transformRect.position.x += canvasSize.x / 2.0f;
		break;
	case Utility::AlignmentPoint::TopRight:
		transformRect.position.x += canvasSize.x;
		break;
	case Utility::AlignmentPoint::MiddleLeft:
		transformRect.position.y += canvasSize.y / 2.0f;
		break;
	case Utility::AlignmentPoint::MiddleCenter:
		transformRect.position.x += canvasSize.x / 2.0f;
		transformRect.position.y += canvasSize.y / 2.0f;
		break;
	case Utility::AlignmentPoint::MiddleRight:
		transformRect.position.x += canvasSize.x;
		transformRect.position.y += canvasSize.y / 2.0f;
		break;
	case Utility::AlignmentPoint::BottomLeft:
		transformRect.position.y += canvasSize.y;
		break;
	case Utility::AlignmentPoint::BottomCenter:
		transformRect.position.x += canvasSize.x / 2.0f;
		transformRect.position.y += canvasSize.y;
		break;
	case Utility::AlignmentPoint::BottomRight:
		transformRect.position.x += canvasSize.x;
		transformRect.position.y += canvasSize.y;
		break;
	default:
		break;
	}

	// キャンバスの表示倍率
	float canvasRatio = pCanvas->GetRatio();
	// 表示倍率を適用
	transformRect.position *= canvasRatio;
	transformRect.size *= canvasRatio;
	// フォントサイズ
	float fontSize = pText->GetFontSize() * canvasRatio;

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

	// 行の上端とベースラインの距離
	float baseline{};
	switch (pText->GetParagraphAlignment())
	{
	case DWRITE_PARAGRAPH_ALIGNMENT_NEAR:
		// 行の上端に文字の上端ががつくようにする
		baseline = fontSize;
		break;
	case DWRITE_PARAGRAPH_ALIGNMENT_CENTER:
		// 行の中心に文字の中心がつくようにする
		baseline = fontSize * (0.4f + pText->GetLineSpace() / 2.0f);
		break;
	case DWRITE_PARAGRAPH_ALIGNMENT_FAR:
		// 行の下端に文字の下端がつくようにする
		baseline = fontSize * (pText->GetLineSpace() - 0.2f);
		break;
	default:
		break;
	}

	// 行間を設定
	textFormat->SetLineSpacing(DWRITE_LINE_SPACING_METHOD_UNIFORM, fontSize * pText->GetLineSpace(), baseline);

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
		transformRect.size.x,
		transformRect.size.y,
		textLayout.GetAddressOf()
	);

	// ブラシ
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
	m_pContext->CreateSolidColorBrush
	(
		pText->GetD2D1FontColor(),
		brush.GetAddressOf()
	);

	// テキスト詳細
	DWRITE_TEXT_METRICS metrics{};
	textLayout->GetMetrics(&metrics);

	// デバッグ表示
	if (isDebug)
	{
		m_pContext->DrawRectangle(transformRect.CreateD2D1_RECT_F(), brush.Get());
		// テキストの長方形
		Math::Rect textRect
		{
			transformRect.position - transformRect.size / 2.0f + Math::Vector2{ metrics.left + metrics.width / 2.0f, metrics.top + metrics.height / 2.0f },
			Math::Vector2{ metrics.width, metrics.height }
		};
		m_pContext->DrawRectangle(textRect.CreateD2D1_RECT_F(), brush.Get());
	}

	// アウトライン幅
	float outlineWidth = pText->GetOutlineWidth() * canvasRatio;

	// アウトライン幅があるなら
	if (outlineWidth != 0.0f)
	{
		// ビットマップの詳細
		D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(
			D2D1_BITMAP_OPTIONS_TARGET,
			D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
		);

		// ビットマップサイズ
		D2D1_SIZE_U bitmapSize
		{
			Math::Min(static_cast<UINT32>(metrics.width + outlineWidth * 2.0f + 2.0f), m_pBackBuffer->GetPixelSize().width),
			Math::Min(static_cast<UINT32>(metrics.height + outlineWidth * 2.0f + 2.0f), m_pBackBuffer->GetPixelSize().height)
		};

		// テキスト用ビットマップ
		Microsoft::WRL::ComPtr<ID2D1Bitmap1> textBitmap{};
		m_textContext->CreateBitmap
		(
			bitmapSize,
			nullptr,
			0,
			bitmapProperties,
			textBitmap.GetAddressOf()
		);

		// ターゲットを設定
		m_textContext->SetTarget(textBitmap.Get());

		// 描画開始
		m_textContext->BeginDraw();
		// 透明に

		// 文字を描画
		DrawTextLayout
		(
			Math::Vector2
			{
				outlineWidth + 1.0f - metrics.left,
				outlineWidth + 1.0f - metrics.top
			},
			m_textContext.Get(),
			textLayout.Get(),
			brush.Get(),
			pText
		);

		// 描画終了
		m_textContext->EndDraw();

		// 膨張エフェクト
		m_dilateEffect->SetInput(0, textBitmap.Get());
		m_dilateEffect->SetValue(D2D1_MORPHOLOGY_PROP_WIDTH, 1 + 2 * Math::RoundInt(outlineWidth));
		m_dilateEffect->SetValue(D2D1_MORPHOLOGY_PROP_HEIGHT, 1 + 2 * Math::RoundInt(outlineWidth));
		Microsoft::WRL::ComPtr<ID2D1Image> dilateImage;
		m_dilateEffect->GetOutput(dilateImage.GetAddressOf());
		// 色変更エフェクト
		m_floodEffect->SetValue(D2D1_FLOOD_PROP_COLOR, pText->GetD2D1OutlineColor());
		Microsoft::WRL::ComPtr<ID2D1Image> floodImage;
		m_floodEffect->GetOutput(floodImage.GetAddressOf());
		// 画像合体エフェクト
		m_compositeEffect->SetInput(0, dilateImage.Get());
		m_compositeEffect->SetInput(1, floodImage.Get());

		// 角度
		float angle = pRectTransform->GetAngle();
		// 描画ターゲットを回転
		if (angle != 0.0f)
		{
			m_pContext->SetTransform(D2D1::Matrix3x2F::Rotation
			(
				angle,
				D2D1::Point2F(transformRect.position.x, transformRect.position.y)
			));
		}

		Math::Vector2 position
		{
			transformRect.position.x - transformRect.size.x / 2.0f - outlineWidth - 1.0f + metrics.left,
			transformRect.position.y - transformRect.size.y / 2.0f - outlineWidth - 1.0f + metrics.top
		};

		// 文字を描画
		m_pContext->DrawImage
		(
			m_compositeEffect.Get(),
			D2D1::Point2F
			(
				position.x,
				position.y
			)
		);
		m_pContext->DrawBitmap
		(
			textBitmap.Get(),
			D2D1::RectF
			(
				position.x,
				position.y,
				position.x + bitmapSize.width,
				position.y + bitmapSize.height
			)
		);

		// コンテキストを元に戻す
		if (angle != 0.0f)
		{
			m_pContext->SetTransform(D2D1::Matrix3x2F::Identity());
		}
	}
	else
	{
		// 角度
		float angle = pRectTransform->GetAngle();
		// 描画ターゲットを回転
		if (angle != 0.0f)
		{
			m_pContext->SetTransform(D2D1::Matrix3x2F::Rotation
			(
				angle,
				D2D1::Point2F(transformRect.position.x, transformRect.position.y)
			));
		}

		// 文字を描画
		DrawTextLayout
		(
			Math::Vector2
			{
				transformRect.position.x - transformRect.size.x / 2.0f,
				transformRect.position.y - transformRect.size.y / 2.0f
			},
			m_pContext,
			textLayout.Get(),
			brush.Get(),
			pText
		);

		// コンテキストを元に戻す
		if (angle != 0.0f)
		{
			m_pContext->SetTransform(D2D1::Matrix3x2F::Identity());
		}
		m_pContext->SetTransform(D2D1::Matrix3x2F::Identity());
	}
}

// 描画終了
void Renderings::TextRenderer::End()
{
	m_pContext->EndDraw();
}

// ウィンドウサイズ変更時の処理
void Renderings::TextRenderer::OnWindowSizeChanged(ID2D1DeviceContext7* pContext, ID2D1Bitmap1* pBackBuffer)
{
	m_pContext = pContext;
	m_pBackBuffer = pBackBuffer;
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

// 文字を描画
void Renderings::TextRenderer::DrawTextLayout(const Math::Vector2& position, ID2D1DeviceContext7* pContext, IDWriteTextLayout* pTextLayout, ID2D1SolidColorBrush* pBrush, const Text* pText)
{
	// 色付け詳細リスト
	const auto& colorDescList = pText->GetDecorationString(*this).GetColorDescList();

	// エフェクトを追加
	for (const auto& colorDesc : colorDescList)
	{
		pTextLayout->SetDrawingEffect
		(
			colorDesc.effect.Get(),
			DWRITE_TEXT_RANGE
			{
				static_cast<UINT32>(colorDesc.beginIndex),
				static_cast<UINT32>(colorDesc.length)
			}
		);
	}

	// 文字を描画
	m_decorationRenderer.Initialize(pContext, pBrush);
	pTextLayout->Draw
	(
		nullptr,
		&m_decorationRenderer,
		position.x,
		position.y
	);
}
