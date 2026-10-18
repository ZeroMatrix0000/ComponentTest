/*
 * FileName:     NPC.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/18
 *
 * NPC
 */

#pragma once

#include "Scripts/Commons/Components/Component.h"

namespace Components
{
	class Transform;
	class RectTransform;
}
namespace Colliders
{
	class BoxCollider;
	class SphereCollider;
}
namespace Renderings
{
	class ICameraScreen;
	class Text;
}

// NPC
class NPC : public Component
{

public:


	/* メンバ関数 */

	// コンストラクタ
	NPC(const ComponentDesc& desc);

	// 初期化処理
	void Initalize(const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder) override;

	// 更新処理
	void Update(float elapsedTime);

	// 回転を設定
	void SetRotation(const Math::Quaternion* rotation = nullptr);

	// スピン
	void Spin(bool isClockwise);

	// 名前を取得
	const std::wstring& GetName() const { return m_name; }
	// イベント名を取得
	const std::string& GetEventName() const { return m_eventName; }
	// 会話判定距離を取得
	float GetFocusDistance() const { return m_focusDistance; }

	// 中心座標を取得
	const Math::Vector3& GetPosition() const;
	// 当たり判定を取得
	const Math::Box& GetBox() const;
	// 当たり判定を取得
	const Math::Sphere& GetSphere() const;

	// スピン中かどうか
	bool IsSpinning() const { return m_spinAngle.IsMoving(); }


private:


	/* メンバ変数 */

	// 名前
	std::wstring m_name;
	// イベント名
	std::string m_eventName;
	// 会話判定距離
	float m_focusDistance;

	// 基準回転
	Math::Quaternion m_baseRotation;

	// 回転
	Spring<Math::Quaternion> m_rotation;
	// スピン回転
	Easing::Value<float> m_spinAngle;

	// トランスフォーム
	Transform* m_pTransform;
	// 長方形の当たり判定
	Colliders::BoxCollider* m_pBoxCollider;
	// 球の当たり判定
	Colliders::SphereCollider* m_pSphereCollider;

};
