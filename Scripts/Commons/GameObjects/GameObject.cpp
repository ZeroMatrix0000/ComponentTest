/*
 * FileName:     GameObject.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/23
 *
 * ゲームオブジェクト
 */

#include "Pch.h"
#include "GameObject.h"

#include "../Components/Component.h"

// コンストラクタ
GameObjects::GameObject::GameObject(Ref<IComponentManager> refIComponentManager)
	: m_name{}
	, m_components{}
	, m_refComponents{}
	, m_refIComponentManager{ refIComponentManager }
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
