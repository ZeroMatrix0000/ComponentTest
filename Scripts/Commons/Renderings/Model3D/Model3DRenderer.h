/*
 * FileName:     Model3DRenderer.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/06
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
		void Render();

		// モデルのポインタを追加
		void AddPModel(const Model3D* pModel) override;
		// モデルのポインタを削除
		void RemovePModel(const Model3D* pModel) override;


	private:


		/* メンバ関数 */

		// 入力レイアウトを作成
		void CreateInputLayout(const Model3DSource* pModelSource);


		/* メンバ変数 */

		// 法線マップエフェクト
		std::unique_ptr<DirectX::NormalMapEffect> m_effect;

		// インスタンスバッファ
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_instanceBuffer;

		// 入力レイアウトリスト
		std::unordered_map<const Model3DSource*, std::vector<std::vector<Microsoft::WRL::ComPtr<ID3D11InputLayout>>>> m_inputLayouts;

		// モデルのポインタリスト
		std::unordered_map<const Model3DSource*, std::vector<const Model3D*>> m_pModels;

		// デバイスのポインタ
		ID3D11Device5* m_pDevice;
		// コンテキストのポインタ
		ID3D11DeviceContext4* m_pContext;
		// コモンステートのポインタ
		const DirectX::CommonStates* m_pCommonStates;

	};
}
