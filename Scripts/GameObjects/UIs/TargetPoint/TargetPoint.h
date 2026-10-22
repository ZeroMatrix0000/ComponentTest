/*
 * FileName:     TargetPoint.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/21
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
	class BoxCollider;
	class SphereCollider;
	class MeshCollider;
}
namespace Renderings
{
	class ICameraScreen;
	class Image;
}

// ターゲット照準
class TargetPoint : public Component
{

public:


	/* 列挙型 */

	// 種類
	enum class Type
	{
		Object,
		Ground,
		Air
	};


	/* 構造体 */

	// 情報
	struct Info
	{
		// ターゲットの位置
		Math::Vector3 position{};
		// 種類
		Type type{};
	};


	/* メンバ関数 */

	// コンストラクタ
	TargetPoint(const ComponentDesc& desc);

	// 初期化処理
	void Initalize(const nlohmann::ordered_json& json, Ref<IGameObjectFinder> iGameObjectFinder) override;

	// 更新処理
	void Update(float elapsedTime);

	// ターゲットを表示
	void Appear(bool isAppear);

	// ターゲット情報を取得
	const Info& GetTargetInfo();


private:


	/* メンバ変数 */

	// ターゲットにできる距離
	float m_targetDistance;
	// レイの長さ
	float m_rayLength;

	// 点滅間隔
	float m_brightInterval;
	// 点滅タイマー
	float m_brightTimer;

	// ターゲット情報
	Info m_info;

	// 画像のトランスフォームの参照
	Ref<RectTransform> m_refTransform;
	// 画像の参照
	Ref<Renderings::Image> m_refImage;

	// プレイヤーの参照
	Ref<Colliders::BoxCollider> m_refPlayer;

	// カメラ画面の参照
	Ref<Renderings::ICameraScreen> m_refICameraScreen;

	// ターゲットリスト
	std::vector<Colliders::SphereCollider*> m_pTargetObjects;

	// 地面リスト
	std::vector<Colliders::MeshCollider*> m_pGrounds;

};
