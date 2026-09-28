/*
 * FileName:     TextOutLineRenderer.h
 * Author:       Takao Hayata
 * Last Updated: 2026/09/25
 *
 * テキストのアウトライン描画
 */

#pragma once

namespace Renderings
{
	class Text;

	// テキストのアウトライン描画
	class TextOutlineRenderer : public IDWriteTextRenderer
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		TextOutlineRenderer
		(
			float                 canvasRatio,
			ID2D1Factory*         pFactory,
			ID2D1RenderTarget*    pRenderTarget,
			ID2D1SolidColorBrush* pTextBrush,
			ID2D1SolidColorBrush* pOutlineBrush,
			ID2D1StrokeStyle*     pStrokeStyle,
			const Text&           text
		);

		// 描画
		HRESULT STDMETHODCALLTYPE DrawGlyphRun
		(
			void*                               clientDrawingContext,
			FLOAT                               baselineOriginX,
			FLOAT                               baselineOriginY,
			DWRITE_MEASURING_MODE               measuringMode,
			const DWRITE_GLYPH_RUN*             glyphRun,
			const DWRITE_GLYPH_RUN_DESCRIPTION* glyphRunDescription,
			IUnknown*                           clientDrawingEffect
		) override;

		HRESULT DrawInlineObject
		(
			void*                clientDrawingContext,
			FLOAT                originX,
			FLOAT                originY,
			IDWriteInlineObject* inlineObject,
			BOOL                 isSideways,
			BOOL                 isRightToLeft,
			IUnknown*            clientDrawingEffect
		) override
		{
			return E_NOTIMPL;
		}

		HRESULT DrawStrikethrough
		(
			void*                       clientDrawingContext,
			FLOAT                       baselineOriginX,
			FLOAT                       baselineOriginY,
			DWRITE_STRIKETHROUGH const* strikethrough,
			IUnknown*                   clientDrawingEffect
		) override 
		{
			return E_NOTIMPL;
		}

		HRESULT DrawUnderline
		(
			void*                   clientDrawingContext,
			FLOAT                   baselineOriginX,
			FLOAT                   baselineOriginY,
			DWRITE_UNDERLINE const* underline,
			IUnknown*               clientDrawingEffect
		) override
		{
			return E_NOTIMPL;
		}

		// 現在の座標系を取得
		HRESULT GetCurrentTransform(void* clientDrawingContext, DWRITE_MATRIX* transform) override;

		// DPI を取得
		HRESULT GetPixelsPerDip(void* clientDrawingContext, FLOAT* pixelsPerDip) override;

		HRESULT QueryInterface(REFIID riid, void** ppvObject) override { return E_NOTIMPL; };

		ULONG AddRef() override { return 0; }
		ULONG Release() override { return 0; }

		// 描画開始
		void Begin();
		// 描画終了
		void End();

	private:


		/* 構造体 */

		// グリフのキー
		struct GlyphKey
		{
			// フォントの顔
			IDWriteFontFace* fontFace{};
			// グリフ番号
			UINT16 glyphIndex{};
			// フォントサイズ
			float fontSize{};
			// 垂直書き込みかどうか
			bool isSideways{};

			bool operator==(const GlyphKey& glyphKey) const
			{
				return
					fontFace == glyphKey.fontFace &&
					glyphIndex == glyphKey.glyphIndex &&
					fontSize == glyphKey.fontSize &&
					isSideways == glyphKey.isSideways
				;
			}
		};

		// グリフキーのハッシュ関数
		struct GlyphKeyHash
		{
			size_t operator()(const GlyphKey& glyphKey) const
			{
				size_t seed = 0;
				HashCombine(&seed, glyphKey.fontFace);
				HashCombine(&seed, glyphKey.glyphIndex);
				HashCombine(&seed, glyphKey.fontSize);
				HashCombine(&seed, glyphKey.isSideways);
				return seed;
			}

		private:

			// ハッシュ合成
			template<typename T>
			static void HashCombine(size_t* pSeed, const T& value)
			{
				// 0x9e3779b9 = 2^32 / φ
				*pSeed ^= std::hash<T>{}(value) + 0x9e3779b9 + (*pSeed << 6) + (*pSeed >> 2);
			}
		};


		/* 静的変数 */

		// グリフのキャッシュ
		static std::unordered_map<GlyphKey, Microsoft::WRL::ComPtr<ID2D1PathGeometry>, GlyphKeyHash> s_glyphCache;


		/* メンバ関数 */

		// ピクセルスナップ設定
		HRESULT IsPixelSnappingDisabled
		(
			void* clientDrawingContext,
			BOOL* isDisabled
		) override;

		// グリフの取得
		ID2D1PathGeometry* GetGlyphGeometry
		(
			IDWriteFontFace* fontFace,
			UINT16 glyphIndex,
			float fontSize,
			bool isSideways
		);

		// グリフの描画
		void DrawGlyphGeometry(ID2D1PathGeometry* pGeometry, const Math::Vector2& position);


		/* メンバ変数 */

		// ジオメトリ
		Microsoft::WRL::ComPtr<ID2D1PathGeometry> m_geometry;
		// 曲線
		Microsoft::WRL::ComPtr<ID2D1GeometrySink> m_sink;

		// キャンバスの表示倍率
		float m_canvasRatio;

		// ファクトリのポインタ
		ID2D1Factory* m_pFactory;

		// 描画ターゲットのポインタ
		ID2D1RenderTarget* m_pRenderTarget;

		// テキストブラシ
		ID2D1SolidColorBrush* m_pTextBrush;
		// アウトラインブラシ
		ID2D1SolidColorBrush* m_pOutlineBrush;
		// 線のスタイル
		ID2D1StrokeStyle* m_pStrokeStyle;

		// テキストのポインタ
		const Text& m_refText;

	};
}
