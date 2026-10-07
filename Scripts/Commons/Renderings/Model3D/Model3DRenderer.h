/*
 * FileName:     Model3DRenderer.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/07
 *
 * 3Dモデル描画
 */

#pragma once

#include "IModel3DRenderer.h"

namespace Systems
{
	class IResources;
}

namespace Renderings
{
	class Model3DSource;
	class VertexShader;

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

		// インスタンシング頂点シェーダを設定
		void SetInstancingVS(const VertexShader* pVertexShader);

		// モデルのポインタを追加
		void AddPModel(const ICameraScreen* pICameraScreen, const Model3D* pModel) override;
		// モデルのポインタを削除
		void RemovePModel(const ICameraScreen* pICameraScreen, const Model3D* pModel) override;


	private:


		/* メンバ関数 */

		// 入力レイアウトを作成
		void CreateInputLayout();

		// インスタンスバッファを更新
		void UpdateInstanceBuffer(const std::vector<DirectX::XMFLOAT3X4>& matrices);

		// インスタンス描画
		void DrawInstanced(const ICameraScreen* pICameraScreen, const DirectX::ModelMeshPart* pPart, size_t matricesCount);


		/* メンバ変数 */

		// インスタンスバッファ
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_instanceBuffer;

		// 入力レイアウト
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

		// モデルのポインタリスト
		std::unordered_map<const ICameraScreen*, std::unordered_map<const Model3DSource*, std::vector<const Model3D*>>> m_pModels;

		// デバイスのポインタ
		ID3D11Device5* m_pDevice;
		// コンテキストのポインタ
		ID3D11DeviceContext4* m_pContext;
		// コモンステートのポインタ
		const DirectX::CommonStates* m_pCommonStates;

		// 頂点シェーダのポインタ
		const VertexShader* m_pInstancingVS;

	};
}
