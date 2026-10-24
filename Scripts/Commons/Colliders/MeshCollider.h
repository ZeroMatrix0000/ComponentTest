/*
 * FileName:     MeshCollider.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/24
 *
 * メッシュの当たり判定
 */

#pragma once

#include "../Components/Component.h"

namespace Components
{
	class Transform;
}
namespace Renderings
{
	class IColliderRenderer;
	class ICameraScreen;
}
namespace Systems
{
	class IResources;
}

namespace Colliders
{
	// 長方形の当たり判定
	class MeshCollider : public Component
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		MeshCollider
		(
			const ComponentDesc& desc,
			Ref<Renderings::IColliderRenderer> iColliderRendererRef,
			Ref<Systems::IResources> iResourcesRef
		);
		// デストラクタ
		~MeshCollider();

		// 初期化処理
		void Initalize(const nlohmann::ordered_json& json, Ref<IGameObjectFinder> iGameObjectFinderRef) override;

		// 色を設定
		void SetColor(const Math::Color& color) { m_color = color; }
		// 色を設定
		void SetColor(const DirectX::XMVECTORF32& color) { m_color = color; }

		// メッシュを設定
		void SetMesh(const std::string& meshName);

		// 映るカメラ画面の参照を追加
		void AddICameraScreenRef(Ref<const Renderings::ICameraScreen> iCameraScreenRef);
		// 映るカメラ画面の参照を削除
		void RemoveICameraScreenRef(Ref<const Renderings::ICameraScreen> iCameraScreenRef);

		// トランスフォームを適用
		void ApplyTransform();

		// ワールド長方形を取得
		const Mesh& GetWorldMesh() const { return m_worldMesh; }

		// 色を取得
		const Math::Color& GetColor() const { return m_color; }

		// 映るカメラ画面の参照リストを取得
		const std::vector<Ref<const Renderings::ICameraScreen>>& GetICameraScreenRefs() const { return m_iCameraScreenRefs; }

	private:


		/* メンバ変数 */

		// 色
		Math::Color m_color;

		// ワールドメッシュ
		Mesh m_worldMesh;

		// メッシュの参照
		Ref<const Mesh> m_meshRef;

		// トランスフォームの参照
		Ref<Transform> m_transformRef;

		// 映るカメラ画面の参照リスト
		std::vector<Ref<const Renderings::ICameraScreen>> m_iCameraScreenRefs;

		// 当たり判定描画インタフェースの参照
		Ref<Renderings::IColliderRenderer> m_iColliderRendererRef;

		// リソース管理の参照
		Ref<Systems::IResources> m_iResourcesRef;

	};
}
