/*
 * FileName:     TargetPoint.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/14
 *
 * ターゲット照準
 */

#pragma once

#include "Scripts/Commons/Components/Component.h"

namespace Components
{
	class RectTransform;
}
namespace Colliders
{
	class SphereCollider;
}
namespace Renderings
{
	class ICameraScreen;
	class Image;
}

// 話しかける・調べる際のカーソル
class TargetPoint : public Component
{

public:


	/* メンバ関数 */

	// コンストラクタ
	TargetPoint(const ComponentDesc& desc);

	// 初期化処理
	void Initalize(const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder) override;

	// 更新処理
	void Update(float elapsedTime);

	// ターゲットを設定
	void SetTarget(const Math::Vector3& playerPosition, const Renderings::ICameraScreen& iCameraScreen);

	// ターゲットを表示
	void Appear(bool isAppear);


private:


	/* メンバ変数 */

	// ターゲットにできる距離
	float m_targetDistance;

	// 点滅間隔
	float m_brightInterval;
	// 点滅タイマー
	float m_brightTimer;

	// 画像のトランスフォーム
	RectTransform* m_pTransform;
	// 画像
	Renderings::Image* m_pImage;

	// ターゲットリスト
	std::vector<Colliders::SphereCollider*> m_pTargets;

	// 照準を合わせるターゲット
	Colliders::SphereCollider* m_pTarget;

};
