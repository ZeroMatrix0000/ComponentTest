/*
 * FileName:     ColliderRenderer.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/14
 *
 * 当たり判定描画
 */

#pragma once

#include "IColliderRenderer.h"

namespace Renderings
{
	// 当たり判定描画
	class ColliderRenderer : public IColliderRenderer
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		ColliderRenderer();

		// 初期化処理
		void Initialize(ID3D11Device5* pDevice, ID3D11DeviceContext4* pContext, const DirectX::CommonStates& commonStates);
		// 描画処理
		void Render();

		// 長方形の当たり判定のポインタを追加
		void AddPBoxCollider(const Colliders::BoxCollider* pBoxCollider) override;
		// 長方形の当たり判定のポインタを削除
		void RemovePBoxCollider(const Colliders::BoxCollider* pBoxCollider) override;

		// 球の当たり判定のポインタを追加
		void AddPSphereCollider(const Colliders::SphereCollider* pSphereCollider) override;
		// 球の当たり判定のポインタを削除
		void RemovePSphereCollider(const Colliders::SphereCollider* pSphereCollider) override;

		// メッシュの当たり判定のポインタを追加
		void AddPMeshCollider(const Colliders::MeshCollider* pMeshCollider) override;
		// メッシュの当たり判定のポインタを削除
		void RemovePMeshCollider(const Colliders::MeshCollider* pMeshCollider) override;


	private:


		/* メンバ関数 */

		// 直方体を描画
		void DrawBox(const Math::Box& box, const Math::Color& color) const;
		// 球を描画
		void DrawSphere(const Math::Sphere& sphere, const Math::Color& color, const Math::Vector3& eyePosition, int segmentCount) const;
		// 円を描画
		void DrawCircle(const Math::Circle& circle, const Math::Color& color, int segmentCount) const;
		// 三角形を描画
		void DrawTriangle(const Math::Triangle& triangle, const Math::Color& color) const;
		// メッシュを描画
		void DrawMesh(const Mesh& mesh, const Math::Color& color) const;


		/* メンバ変数 */

		// プリミティブバッチ
		std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_primitiveBatch;
		// ベーシックエフェクト
		std::unique_ptr<DirectX::BasicEffect>                                  m_basicEffect;

		// 長方形の当たり判定のポインタリスト
		std::vector<const Colliders::BoxCollider*>    m_pBoxColliders;
		// 球の当たり判定のポインタリスト
		std::vector<const Colliders::SphereCollider*> m_pSphereColliders;
		// メッシュの当たり判定のポインタリスト
		std::vector<const Colliders::MeshCollider*>   m_pMeshColliders;

		// デバイスコンテキスト
		ID3D11DeviceContext4*        m_pContext;
		// コモンステート
		const DirectX::CommonStates* m_pCommonStates;

		// 入力レイアウト
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

	};
}
