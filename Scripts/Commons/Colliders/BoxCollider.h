/*
 * FileName:     BoxCollider.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/22
 *
 * 長方形の当たり判定
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
	// 長方形の当たり判定
	class BoxCollider : public Component
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		BoxCollider(const ComponentDesc& desc, Ref<Renderings::IColliderRenderer> iColliderRenderer);
		// デストラクタ
		~BoxCollider();

		// 初期化処理
		void Initalize(const nlohmann::ordered_json& json, Ref<IGameObjectFinder> iGameObjectFinder) override;

		// 座標を設定
		void SetPosition(const Math::Vector3& position) { m_box.position = position; }
		// 大きさを設定
		void SetSize(const Math::Vector3& size) { m_box.size = size; }
		// 色を設定
		void SetColor(const Math::Color& color) { m_color = color; }
		// 色を設定
		void SetColor(const DirectX::XMVECTORF32& color) { m_color = color; }

		// 映るカメラ画面を追加
		void AddICameraScreen(Ref<const Renderings::ICameraScreen> iCameraScreen);
		// 映るカメラ画面を削除
		void RemoveICameraScreen(Ref<const Renderings::ICameraScreen> iCameraScreen);

		// トランスフォームを適用
		void ApplyTransform();

		// ワールド長方形を取得
		const Math::Box& GetWorldBox() const { return m_worldBox; }

		// 色を取得
		const Math::Color& GetColor() const { return m_color; }

		// 映るカメラ画面のポインタリストを取得
		const std::vector<Ref<const Renderings::ICameraScreen>>& GetRefICameraScreens() const { return m_refICameraScreens; }

	private:


		/* メンバ変数 */

		// 長方形
		Math::NonRotatingBox m_box;

		// ワールド長方形
		Math::Box m_worldBox;

		// 色
		Math::Color m_color;

		// トランスフォームの参照
		Ref<Transform> m_refTransform;

		// 映るカメラ画面の参照リスト
		std::vector<Ref<const Renderings::ICameraScreen>> m_refICameraScreens;

		// 当たり判定描画インタフェースの参照
		Ref<Renderings::IColliderRenderer> m_refIColliderRenderer;

	};
}
