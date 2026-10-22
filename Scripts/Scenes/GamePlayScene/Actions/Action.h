/*
 * FileName:     Action.h
 * Author:       Takao Hayata
 * Last Updated: 2026/09/10
 *
 * アクション
 */

#pragma once

#include "Scripts/Commons/Components/Component.h"

class IGameInput;

// アクション
class Action : public Component
{

public:


	/* メンバ関数 */

	// コンストラクタ
	Action(const ComponentDesc& desc);
	// デストラクタ
	virtual ~Action() = default;

	// 初期化処理
	void Initalize(const nlohmann::ordered_json& json, Ref<IGameObjectFinder> iGameObjectFinder) override;

	// パラメータを設定
	virtual void SetParams(const nlohmann::ordered_json& json, Ref<IGameObjectFinder> iGameObjectFinder);

	// 開始処理
	virtual void Enter();
	// 更新処理
	virtual void Update(float elapsedTime, const IGameInput& iGameInput);
	// 終了処理
	virtual void Exit();

	// 次のアクションに移動するかどうかを取得
	bool IsNext() const { return m_isNext; }


protected:


	/* メンバ関数 */

	// 次のアクションに移動
	void GoNext() { m_isNext = true; }


private:


	/* メンバ変数 */

	// 次のアクションに移動するかどうか
	bool m_isNext;

};