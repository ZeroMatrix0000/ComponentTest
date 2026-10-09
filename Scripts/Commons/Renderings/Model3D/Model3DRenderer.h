/*
 * FileName:     Model3DRenderer.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/09
 *
 * 3Dモデル描画
 */

#pragma once

#include "IModel3DRenderer.h"
#include "Model3DSource.h"

namespace Systems
{
	class IResources;
}

namespace Renderings
{
	class VertexShader;
	class PixelShader;

	// 3Dモデル描画
	class Model3DRenderer : public IModel3DRenderer
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		Model3DRenderer();

		// 初期化処理
		void Initialize(ID3D11Device5* pDevice, ID3D11DeviceContext4* pContext, const DirectX::CommonStates& commonStates);
		// 描画処理
		void Render(bool isInstance);

		// 頂点シェーダを設定
		void SetVertexShader(const VertexShader* pVertexShader);
		// ピクセルシェーダを設定
		void SetPixelShader(const PixelShader* pPixelShader) { m_pPixelShader = pPixelShader; }

		// モデルのポインタを追加
		void AddPModel(const ICameraScreen* pICameraScreen, const Model3D* pModel) override;
		// モデルのポインタを削除
		void RemovePModel(const ICameraScreen* pICameraScreen, const Model3D* pModel) override;

		// パーティクルのポインタを追加
		void AddPParticle(const ICameraScreen* pICameraScreen, const Particle::RenderElement* pParticleElement) override;
		// パーティクルのポインタを削除
		void RemovePParticle(const ICameraScreen* pICameraScreen, const Particle::RenderElement* pParticleElement) override;


	private:


		/* 構造体 */

		// 描画オブジェクト
		struct RenderObjects
		{
			// モデルのポインタリスト
			std::vector<const Model3D*> pModels{};
			// パーティクルのポインタリスト
			std::vector<const Particle::RenderElement*> pParticleElements{};
		};


		/* メンバ関数 */

		// 白画像を作成
		void CreateWhiteTexture();

		// 入力レイアウトを作成
		void CreateInputLayout();

		// インスタンスバッファを更新
		void UpdateInstanceBuffer(const std::vector<DirectX::XMFLOAT3X4>& matrices);

		// パーツの情報を設定
		void SetPartInfo(const Model3DSource::PartInfo& partInfo);

		// 描画
		void Draw(const ICameraScreen* pICameraScreen, const DirectX::ModelMeshPart* pPart);
		// インスタンス描画
		void DrawInstanced(const ICameraScreen* pICameraScreen, const DirectX::ModelMeshPart* pPart, size_t instMatricesCount);


		/* メンバ変数 */

		// インスタンスバッファ
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_instanceBuffer;

		// 入力レイアウト
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

		// 白画像
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_whiteTexture;

		// 描画オブジェクトリスト
		std::unordered_map<const ICameraScreen*, std::unordered_map<const Model3DSource*, RenderObjects>> m_renderObjects;

		// デバイスのポインタ
		ID3D11Device5* m_pDevice;
		// コンテキストのポインタ
		ID3D11DeviceContext4* m_pContext;
		// コモンステートのポインタ
		const DirectX::CommonStates* m_pCommonStates;

		// 頂点シェーダのポインタ
		const VertexShader* m_pVertexShader;
		// ピクセルシェーダのポインタ
		const PixelShader* m_pPixelShader;

	};
}
