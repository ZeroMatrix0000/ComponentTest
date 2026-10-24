/*
 * FileName:     SphereCollider.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/24
 *
 * 球の当たり判定
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

namespace Colliders
{
	// 球の当たり判定
	class SphereCollider : public Component
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		SphereCollider(const ComponentDesc& desc, Ref<Renderings::IColliderRenderer> iColliderRendererRef);
		// デストラクタ
		~SphereCollider();

		// 初期化処理
		void Initalize(const nlohmann::ordered_json& json, Ref<IGameObjectFinder> iGameObjectFinderRef) override;

		// 座標を設定
		void SetPosition(const Math::Vector3& position) { m_sphere.centerPosition = position; }
		// 半径を設定
		void SetRadius(float radius) { m_sphere.radius = radius; }
		// 色を設定
		void SetColor(const Math::Color& color) { m_color = color; }
		// 色を設定
		void SetColor(const DirectX::XMVECTORF32& color) { m_color = color; }

		// 映るカメラ画面の参照を追加
		void AddICameraScreenRef(Ref<const Renderings::ICameraScreen> iCameraScreenRef);
		// 映るカメラ画面の参照を削除
		void RemoveICameraScreenRef(Ref<const Renderings::ICameraScreen> iCameraScreenRef);

		// トランスフォームを適用
		void ApplyTransform();

		// ワールド球を取得
		const Math::Sphere& GetWorldSphere() const { return m_worldSphere; }

		// 色を取得
		const Math::Color& GetColor() const { return m_color; }

		// 映るカメラ画面の参照リストを取得
		const std::vector<Ref<const Renderings::ICameraScreen>>& GetICameraScreenRefs() const { return m_iCameraScreenRefs; }

	private:


		/* メンバ変数 */

		// 長方形
		Math::Sphere m_sphere;

		// ワールド長方形
		Math::Sphere m_worldSphere;

		// 色
		Math::Color m_color;

		// トランスフォームの参照
		Ref<Transform> m_refTransform;

		// 映るカメラ画面の参照リスト
		std::vector<Ref<const Renderings::ICameraScreen>> m_iCameraScreenRefs;

		// 当たり判定描画インタフェースの参照
		Ref<Renderings::IColliderRenderer> m_iColliderRendererRef;

	};
}
