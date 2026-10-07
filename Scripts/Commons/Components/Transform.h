/*
 * FileName:     Transform.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/07
 *
 * トランスフォーム
 */

#pragma once

#include "Component.h"

namespace Components
{
	// トランスフォーム
	class Transform : public Component
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		Transform(const ComponentDesc& desc);

		// 初期化処理
		void Initalize(const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder) override;

		// 座標を設定
		void SetPosition(const Math::Vector3& position);
		// 回転を設定
		void SetRotation(const Math::Quaternion& rotation);
		// 拡大を設定
		void SetScale(const Math::Vector3& scale);

		// 座標を足す
		void Translate(const Math::Vector3& displacement);
		
		// 座標を取得
		const Math::Vector3& GetPosition()    const { return m_position; }
		// 回転を取得
		const Math::Quaternion& GetRotation() const { return m_rotation; }
		// 拡大を取得
		const Math::Vector3& GetScale()       const { return m_scale; }

		// ワールド行列を作成
		const Math::Matrix& GetWorldMatrix() const;


	private:


		/* メンバ関数 */

		// 行列を更新
		void UpdateWorldMatrix();


		/* メンバ変数 */

		// 座標
		Math::Vector3 m_position;
		// 回転
		Math::Quaternion m_rotation;
		// 拡大
		Math::Vector3 m_scale;

		// ワールド行列
		mutable Math::Matrix m_matrix;
		// 変更があったか
		mutable bool m_isChanged;

	};
}
