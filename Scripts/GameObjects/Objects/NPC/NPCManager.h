/*
 * FileName:     NPCManager.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/21
 *
 * NPC管理
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
}
class NPC;
class InteractCursor;
class Player;

// NPC管理
class NPCManager : public Component
{

public:


	/* メンバ関数 */

	// コンストラクタ
	NPCManager(const ComponentDesc& desc);

	// 初期化処理
	void Initalize(const nlohmann::ordered_json& json, Ref<IGameObjectFinder> iGameObjectFinder) override;

	// 更新処理
	void Update(float elapsedTime);

	// 向きを設定
	void SetRotation(const Math::Vector3& playerPosition);

	// カーソルを設定
	void SetCursor(const Renderings::ICameraScreen& iCameraScreen);

	// カーソルを隠す
	void HideCursor(bool isHideCursor) { m_isHideCursor = isHideCursor; }

	// プレイヤーとの当たり判定
	void PlayerCollide(Player* pPlayer);

	// 当たり判定を取得
	std::vector<const Math::Box*> GetPBoxes() const;

	// カーソルが合っているNPCを取得
	const NPC* GetPFocusedNPC() const { return m_pFocusedNPC; }


private:


	/* メンバ変数 */

	// カーソルを隠しているか
	bool m_isHideCursor;

	// NPCリスト
	std::vector<NPC*> m_pNPCList;
	// カーソルリスト
	std::vector<Ref<InteractCursor>> m_pInteractCursorList;

	// カーソルが合っているMPC
	NPC* m_pFocusedNPC;

};
