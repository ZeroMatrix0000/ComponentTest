/*
 * FileName:     TargetPoint.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/15
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

// 話しかける・調べる際のカーソル
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
	void Initalize(const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder) override;

	// 更新処理
	void Update(float elapsedTime);

	// ターゲットを表示
	void Appear(bool isAppear);

	// ターゲットを取得
	Colliders::SphereCollider* GetPTargetObject() const { return m_pTargetObject; }

	// ターゲット情報を生成
	Info CreateTargetInfo() const;


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

	// 画像のトランスフォーム
	RectTransform* m_pTransform;
	// 画像
	Renderings::Image* m_pImage;

	// プレイヤー
	Colliders::BoxCollider* m_pPlayer;

	// カメラ画面
	Renderings::ICameraScreen* m_pICameraScreen;

	// ターゲットリスト
	std::vector<Colliders::SphereCollider*> m_pTargetObjects;

	// 地面リスト
	std::vector<Colliders::MeshCollider*> m_pGrounds;

	// 照準を合わせるターゲット
	Colliders::SphereCollider* m_pTargetObject;

};
