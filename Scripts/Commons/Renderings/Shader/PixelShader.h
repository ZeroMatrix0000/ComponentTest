/*
 * FileName:     PixelShader.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/08
 *
 * ピクセルシェーダ
 */

#pragma once

#include "ConstantBuffer.h"

namespace Renderings
{
	class PixelShader
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		PixelShader();

		// 初期化処理
		void Initialize(ID3D11Device5* pDevice, const std::wstring& filePath);

		// ブロブデータを取得
		auto* GetBlob() const { return m_blob.Get(); }
		// シェーダを取得
		auto* GetD3DShader() const { return m_d3dShader.Get(); }
		// 定数バッファを取得
		auto* GetConstantBuffer() const { return m_constantBuffer.get(); }

		// 定数バッファを GPU に送信
		void SetConstantBuffer(ID3D11Device5* pDevice, ID3D11DeviceContext4* pContext) const;

		/* 静的関数 */

		// 生成
		static PixelShader Create(ID3D11Device5* pDevice, const std::wstring& filePath);


	private:


		/* メンバ変数 */

		// ブロブデータ
		Microsoft::WRL::ComPtr<ID3DBlob> m_blob;

		// シェーダ
		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_d3dShader;

		// 定数バッファ
		std::unique_ptr<ConstantBuffer> m_constantBuffer;

	};
}