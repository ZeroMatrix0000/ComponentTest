/*
 * FileName:     GameObject.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/24
 *
 * ゲームオブジェクト
 */

#include "Pch.h"
#include "GameObject.h"

#include "../Components/Component.h"

// コンストラクタ
GameObjects::GameObject::GameObject(Ref<IComponentManager> iComponentManagerRef)
	: m_name{}
	, m_components{}
	, m_componentRefs{}
	, m_iComponentManagerRef{ iComponentManagerRef }
{
}

// メッセージを受け取る
void GameObjects::GameObject::AcceptMessage(const std::string& message) const
{
	for (const auto& component : m_components)
	{
		component.first->AcceptMessage(message);
	}
}
