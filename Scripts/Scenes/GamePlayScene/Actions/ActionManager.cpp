/*
 * FileName:     ActionManager.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/23
 *
 * アクション管理
 */

#include "Pch.h"
#include "ActionManager.h"

#include "Actions.h"
#include "Scripts/Main/IsDerivedRegister.h"
#include "Scripts/Commons/GameObjects/GameObject.h"
#include "Scripts/Commons/Systems/IResources.h"

// コンストラクタ
ActionManager::ActionManager(const ComponentDesc& desc, Systems::IResources* pIResources)
	: Component{ desc }
	, m_eventCount{}
	, m_eventIndex{}
	, m_pEvent{}
	, m_pCurrentAction{}
	, m_pIGameObjectFinder{}
	, m_pIResources{ pIResources }
{
}

// 初期化処理
void ActionManager::Initalize(const nlohmann::ordered_json& json, Ref<IGameObjectFinder> iGameObjectFinder)
{
	m_pIGameObjectFinder = &iGameObjectFinder.Get();
}

// 更新処理
void ActionManager::Update(float elapsedTime, const IGameInput& iGameInput)
{
	// アクションがなければ何もしない
	if (IsCompleted())
	{
		return;
	}

	// アクションを更新
	m_pCurrentAction->Update(elapsedTime, iGameInput);

	// 次のアクションに移動
	if (m_pCurrentAction->IsNext())
	{
		NextAction();
	}
}

// イベントを設定
void ActionManager::LoadEvent(const std::string& eventName)
{
	m_pEvent = m_pIResources->GetJson(eventName);

	m_eventCount = m_pEvent->size();
	m_eventIndex = 0;

	if (!IsCompleted())
	{
		InitializeCurrentAction();
	}
}

// アクション切り替え
void ActionManager::NextAction()
{
	m_pCurrentAction->Exit();
	m_eventIndex++;
	if (!IsCompleted())
	{
		InitializeCurrentAction();
	}
}

// 現在のアクションを初期化
void ActionManager::InitializeCurrentAction()
{
	const auto& eventList = m_pEvent->at(m_eventIndex);

	auto it = eventList.find("ActionType");
	if (it == eventList.end())
	{
		m_pCurrentAction = &GetOwner()->GetComponent<Action>().Get();
	}
	else
	{
		if (it.value() == "OpenMessageWindow")
		{
			m_pCurrentAction = &GetOwner()->GetComponent<ActionOpenMessageWindow>().Get();
		}
		else if (it.value() == "CloseMessageWindow")
		{
			m_pCurrentAction = &GetOwner()->GetComponent<ActionCloseMessageWindow>().Get();
		}
		else if (it.value() == "SetMessageWindowText")
		{
			m_pCurrentAction = &GetOwner()->GetComponent<ActionSetMessageWindowText>().Get();
		}
		else
		{
			m_pCurrentAction = &GetOwner()->GetComponent<Action>().Get();
		}
	}

	it = eventList.find("Params");
	if (it != eventList.end())
	{
		m_pCurrentAction->SetParams(*it, Ref{ *m_pIGameObjectFinder });
	}

	m_pCurrentAction->Enter();
}
