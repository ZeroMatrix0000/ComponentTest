/*
 * FileName:     BlackBelt.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/21
 *
 * 上下の黒帯
 */

#pragma once

#include "Scripts/Commons/Components/Component.h"

namespace Components
{
	class RectTransform;
}
namespace Renderings
{
	class Image;
}

// 上下の黒帯
class BlackBelt : public Component
{

public:


	/* メンバ関数 */

	// コンストラクタ
	BlackBelt(const ComponentDesc& desc);

	// 初期化処理
	void Initalize(const nlohmann::ordered_json& json, Ref<IGameObjectFinder> iGameObjectFinder) override;

	// 更新処理
	void Update(float elapsedTime);

	// 出現
	void Appear(bool isAppear);

	// 幅を設定
	void SetWidth(float width);


private:


	/* メンバ変数 */

	// 移動時間
	float m_moveTime;

	// 高さ
	float m_height;

	// 移動率
	Easing::Value<float> m_moveRate;

	// 上の画像のトランスフォーム
	Ref<RectTransform> m_refTopImageTransform;
	// 上の画像
	Ref<Renderings::Image> m_refTopImage;
	// 下の画像のトランスフォーム
	Ref<RectTransform> m_refBottomImageTransform;
	// 下の画像
	Ref<Renderings::Image> m_refBottomImage;

};
