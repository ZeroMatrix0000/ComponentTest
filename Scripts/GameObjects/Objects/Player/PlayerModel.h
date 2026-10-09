/*
 * FileName:     PlayerModel.h
 * Author:       Takao Hayata
 * Last Updated: 2026/09/11
 *
 * プレイヤーのモデル
 */

#pragma once

#include "Scripts/Commons/Components/Component.h"

namespace Components
{
	class Transform;
}
namespace Renderings
{
	class ParticleManager;
}
class Player;

// プレイヤーのモデル
class PlayerModel : public Component
{

public:


	/* メンバ関数 */

	// コンストラクタ
	PlayerModel(const ComponentDesc& desc);

	// 初期化処理
	void Initalize(const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder) override;

	// 更新処理
	void Update(float elapsedTime, const Player& player);

	// トランスフォームを設定
	void SetTransform(const Transform& transform);

	// パーティクル管理を設定
	void SetParticleManager(Renderings::ParticleManager* pParticleManager) { m_pParticleManager = pParticleManager; }


private:


	/* メンバ変数 */

	// 跳ねる高さ
	float m_bounceHeight;
	// 落下速度係数
	float m_fallSpeedCoefficient;
	// スケール変動の最大値
	float m_scalingMax;
	// ダッシュ時の傾き
	float m_dashAngleMax;

	// 跳ねる周期
	Cycled m_bounceTime;

	// ジャンプまたは落下中の高さ
	Spring<float> m_fallingHeight;
	// スケールの差
	Spring<float> m_scaling;
	// ダッシュ時の傾き
	Spring<float> m_dashAngle;

	// 自身のトランスフォーム
	Transform* m_pTransform;

	// パーティクル管理
	Renderings::ParticleManager* m_pParticleManager;

};
