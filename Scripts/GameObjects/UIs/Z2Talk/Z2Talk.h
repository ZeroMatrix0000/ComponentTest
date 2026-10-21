/*
 * FileName:     Z2Talk.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/21
 *
 * Z: はなす
 */

#pragma once

#include "Scripts/Commons/Components/Component.h"

namespace Components
{
	class RectTransform;
}
namespace Renderings
{
	class ICameraScreen;
	class Text;
}

// Z: はなす
class Z2Talk : public Component
{

public:


	/* メンバ関数 */

	// コンストラクタ
	Z2Talk(const ComponentDesc& desc);

	// 初期化処理
	void Initalize(const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder) override;

	// 更新処理
	void Update(float elapsedTime, bool isFocused);

	// 座標を設定
	void SetPosition(const Math::Box& playerBox, const Renderings::ICameraScreen& iCameraScreen);


private:


	/* メンバ変数 */

	// 不透明度
	Limited m_alpha;
	// 不透明度の速度
	float m_alphaVelocity;

	// トランスフォームの参照
	Ref<RectTransform> m_refRectTransform;
	// テキストの参照
	Ref<Renderings::Text> m_refText;

};
