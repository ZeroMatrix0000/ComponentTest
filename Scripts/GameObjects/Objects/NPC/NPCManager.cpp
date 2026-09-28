/*
 * FileName:     NPCManager.h
 * Author:       Takao Hayata
 * Last Updated: 2026/09/28
 *
 * NPC管理
 */

#include "Pch.h"
#include "NPCManager.h"

#include "Scripts/Commons/Components/RectTransform.h"
#include "Scripts/Commons/Renderings/ICameraScreen.h"
#include "Scripts/Commons/Renderings/Canvas.h"
#include "Scripts/Commons/Renderings/Text/Text.h"
#include "Scripts/Commons/Renderings/Image/Image.h"
#include "Scripts/Commons/Systems/JsonSerializer.h"
#include "NPC.h"
#include "Scripts/GameObjects/UIs/InteractCursor/InteractCursor.h"

// コンストラクタ
NPCManager::NPCManager(const ComponentDesc& desc)
	: Component{ desc }
	, m_isHideCursor{}
	, m_pNPCList{}
	, m_pInteractCursorList{}
	, m_pFocusedNPC{}
{
}

// 初期化処理
void NPCManager::Initalize(const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder)
{
	const Renderings::Canvas* pCanvas = nullptr;

	Systems::JsonSerializer serializer{ pIGameObjectFinder };
	serializer.AddParameter(&m_pNPCList, "NPCList");
	serializer.AddParameter(&pCanvas, "Canvas");
	serializer.Load(json);

	// NPC の数だけテキストを追加
	for (int i = 0; i < m_pNPCList.size(); i++)
	{
		InteractCursor* pInteractCursor = Instantiate("Prefab_InteractCursor")->GetComponent<InteractCursor>();
		m_pInteractCursorList.push_back(pInteractCursor);
		pInteractCursor->SetName(m_pNPCList.at(i)->GetName());
		if (pCanvas)
		{
			pInteractCursor->SetCanvas(*pCanvas);
		}
	}
}

// 更新処理
void NPCManager::Update(float elapsedTime)
{
	for (int i = 0; i < m_pNPCList.size(); i++)
	{
		m_pNPCList.at(i)->Update(elapsedTime);
		m_pInteractCursorList.at(i)->Update(elapsedTime, m_pNPCList.at(i) == m_pFocusedNPC && !m_isHideCursor);
	}
}

// 向きを設定
void NPCManager::SetRotation(const Math::Vector3& playerPosition)
{
	// 距離の最小
	float distanceMin = Math::INFTY;

	m_pFocusedNPC = nullptr;

	for (auto* pNPC : m_pNPCList)
	{
		// プレイヤーとの中心距離（XZ平面）
		float distance = ((playerPosition - pNPC->GetPosition()) * Math::Vector3{ 1.0f, 0.0f, 1.0f }).Length();
		// 距離がある程度近かったら
		if (distance < pNPC->GetFocusDistance())
		{
			// 向く方向
			Math::Vector3 direction = playerPosition - pNPC->GetPosition();
			// Y軸方向は無視
			direction.y = 0.0f;
			// 正規化
			direction.Normalize();
			// 回転
			Math::Quaternion rotation = Math::Quaternion::FromToRotation(Math::Vector3::Backward, direction);
			pNPC->SetRotation(&rotation);

			// 最も近いNPCを記録
			if (distance < distanceMin)
			{
				distanceMin = distance;
				m_pFocusedNPC = pNPC;
			}
		}
		else
		{
			pNPC->SetRotation();
		}
	}
}

// カーソルを設定
void NPCManager::SetCursor(const Renderings::ICameraScreen& iCameraScreen)
{
	for (int i = 0; i < m_pNPCList.size(); i++)
	{
		const Math::Box& box = m_pNPCList.at(i)->GetBox();

		Math::Vector2 screenPosition = Math::Geometry::Projection
		(
			box.position + box.size * Math::Vector3::Up / 2.0f,
			iCameraScreen.GetViewMatrix(),
			iCameraScreen.GetProjectionMatrix(),
			m_pInteractCursorList.at(i)->GetCanvas().GetSize()
		);

		m_pInteractCursorList.at(i)->SetPosition(screenPosition);
	}
}

// 当たり判定を取得
std::vector<const Math::Box*> NPCManager::GetPBoxes() const
{
	std::vector<const Math::Box*> pBoxes{};
	for (const auto* pNPC : m_pNPCList)
	{
		pBoxes.push_back(&pNPC->GetBox());
	}
	return pBoxes;
}
