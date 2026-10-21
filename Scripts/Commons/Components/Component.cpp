/*
 * FileName:     Component.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/21
 *
 * コンポーネント
 */

#include "Pch.h"
#include "Component.h"

#include "ComponentDesc.h"
#include "../GameObjects/IGameObjectInstantiator.h"

// コンストラクタ
Components::Component::Component(const ComponentDesc& desc)
	: m_pOwner{ desc.pOwner }
	, m_pIGameObjectInstantiator{ desc.pIGameObjectInstantiator }
{
}

// 所有者を取得
GameObject& Components::Component::GetOwner()
{
	return m_pOwner ? *m_pOwner : m_pIGameObjectInstantiator->GetNullReferences();
}

// 変更不可所有者を取得
const GameObject& Components::Component::GetConstOwner() const
{
	return m_pOwner ? *m_pOwner : m_pIGameObjectInstantiator->GetNullReferences();
}

// ゲームオブジェクトを生成
GameObject& Components::Component::Instantiate(const std::string& name) const
{
	return m_pIGameObjectInstantiator->Instantiate(name);
}
