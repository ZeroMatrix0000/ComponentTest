/*
 * FileName:     MessageWindow.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/21
 *
 * メッセージウィンドウ
 */

#pragma once

#include "Scripts/Commons/Components/Component.h"

#include "Scripts/Commons/Components/RectTransform.h"
#include "Scripts/Commons/Renderings/Canvas.h"
#include "Scripts/Commons/Renderings/Image/Image.h"
#include "Scripts/Commons/Renderings/Text/Text.h"
#include "Scripts/Commons/Systems/JsonSerializer.h"

namespace Components
{
	class RectTransform;
}
namespace Renderings
{
	class Text;
	class Image;
}

// メッセージウィンドウ
class MessageWindow : public Component
{

public:


	/* メンバ関数 */

	// コンストラクタ
	MessageWindow(const ComponentDesc& desc);

	// 初期化処理
	void Initalize(const nlohmann::ordered_json& json, Ref<IGameObjectFinder> iGameObjectFinder) override;

	// 更新処理
	void Update(float elapsedTime);

	// 出現
	void Appear(bool isAppear);

	// テキストを設定
	void SetText(const std::wstring& text = L"");
	// テキストを全て表示
	void DisplayAllText();

	// 話者を設定
	void SetCharacter(const std::wstring& text = L" ");

	// 動いているかどうか
	bool IsMoving() const { return m_moveRate.IsMoving(); }

	// テキストを全て表示したか
	bool IsDisplayAllText() const { return m_textCount.IsMax(); }


private:


	/* メンバ変数 */

	// 移動時間
	float m_moveTime;
	// 1秒に表示される文字数
	float m_textVelocity;

	// 背景サイズ
	Math::Vector2 m_backgroundSize;
	// キャラクター名サイズ
	Math::Vector2 m_characterSize;
	// テキストサイズ
	Math::Vector2 m_textSize;

	// キャラクター名フォントサイズ
	float m_characterFontSize;
	// テキストフォントサイズ
	float m_textFontSize;

	// 移動率
	Easing::Value<float> m_moveRate;

	// 文字
	std::wstring m_text;
	// 文字数カウント
	Limited m_textCount;

	// 背景のトランスフォームの参照
	Ref<RectTransform> m_refBackgroundTransform;
	// 背景の参照
	Ref<Renderings::Image> m_refBackground;

	// キャラクター名のトランスフォームの参照
	Ref<RectTransform> m_refCharacterTransform;
	// キャラクター名の参照
	Ref<Renderings::Text> m_refCharacter;

	// テキストのトランスフォームの参照
	Ref<RectTransform> m_refTextTransform;
	// テキストの参照
	Ref<Renderings::Text> m_refText;


};
