/*
 * FileName:     Particle.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/09
 *
 * パーティクル
 */

#pragma once

#include "Scripts/Commons/Components/Component.h"

namespace Components
{
	class Transform;
}

namespace Systems
{
	class IResources;
}

namespace Renderings
{
	class Model3DSource;
	class ICameraScreen;
	class IModel3DRenderer;

	// パーティクル
	class Particle
	{

	public:


		/* 構造体 */

		// 更新情報
		struct UpdateElement
		{
			// 座標
			Math::Vector3 position{ Math::Vector3::Zero };
			// 回転
			Math::Quaternion rotation{ Math::Quaternion::Identity };
			// 拡大
			Math::Vector3 scale{ Math::Vector3::One };
		};

		// 描画情報
		struct RenderElement
		{
			// ワールド行列
			Math::Matrix matrix{ Math::Matrix::Identity };
			// モデルソース
			const Model3DSource* pModelSource{};
		};


		/* メンバ関数 */

		// コンストラクタ
		Particle(IModel3DRenderer* pIModelRenderer, const Systems::IResources& iResources);
		// デストラクタ
		virtual ~Particle();

		// 初期化処理
		virtual void Initalize(const Transform* pTransform) = 0;

		// 更新処理
		virtual void Update(float elapsedTime) = 0;

		// ワールド行列を適用
		void ApplyTransform();

		// モデルソースを設定
		void SetModelSource(const std::string& modelSourceName, RenderElement* pRenderElement);

		// 映るカメラ画面を追加
		void AddICameraScreen(const ICameraScreen& iCameraScreen);
		// 映るカメラ画面を削除
		void RemoveICameraScreen(const ICameraScreen& iCameraScreen);

		// アクティブフラグを取得
		bool IsActive() const { return m_isActive; }


	protected:


		/* メンバ関数 */

		// 要素を追加
		void AddElement(UpdateElement** ppUpdateElement, RenderElement** ppRenderElement);

		// 要素を取得
		UpdateElement* GetPUpdateElement(size_t index) { return &m_updateElements.at(index); }

		// 非アクティブ化
		void Destroy() { m_isActive = false; }


	private:


		/* メンバ変数 */

		// 更新情報リスト
		std::vector<UpdateElement> m_updateElements;
		// 描画情報リストリスト
		std::vector<std::unique_ptr<RenderElement>> m_renderElements;

		// アクティブフラグ
		bool m_isActive;

		// 映るカメラ画面のポインタリスト
		std::vector<const ICameraScreen*> m_pICameraScreens;

		// モデル描画インタフェースのポインタ
		IModel3DRenderer* m_pIModelRenderer;

		// リソース管理
		const Systems::IResources& m_refIResources;

	};
}
