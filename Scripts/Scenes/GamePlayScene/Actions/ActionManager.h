/*
 * FileName:     ActionManager.h
 * Author:       Takao Hayata
 * Last Updated: 2026/09/21
 *
 * アクション管理
 */

#pragma once

#include "Scripts/Commons/Components/Component.h"

namespace Systems
{
	class IResources;
}
class IGameInput;
class Action;

// 長方形の当たり判定
class ActionManager : public Component
{

public:


	/* メンバ関数 */

	// コンストラクタ
	ActionManager(const ComponentDesc& desc, Systems::IResources* pIResources);

	// 初期化処理
	void Initalize(const nlohmann::ordered_json& json, Ref<IGameObjectFinder> iGameObjectFinderRef) override;

	// 更新処理
	void Update(float elapsedTime, const IGameInput& iGameInput);

	// イベントを設定
	void LoadEvent(const std::string& eventName);

	// 全てのアクションを終了したかどうか
	bool IsCompleted() const { return m_eventIndex == m_eventCount; }


private:


	/* メンバ関数 */

	// アクション切り替え
	void NextAction();

	// 現在のアクションを初期化
	void InitializeCurrentAction();


	/* メンバ変数 */

	// アクション数
	size_t m_eventCount;
	// 現在のアクション番号
	size_t m_eventIndex;

	// イベント
	const nlohmann::ordered_json* m_pEvent;

	// アクション
	Action* m_pCurrentAction;

	// ゲームオブジェクト検索
	IGameObjectFinder* m_pIGameObjectFinder;

	// リソース管理
	Systems::IResources* m_pIResources;

};
