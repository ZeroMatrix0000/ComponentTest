/*
 * FileName:     Component.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/22
 *
 * コンポーネント
 */

#include "Pch.h"
#include "Component.h"

#include "ComponentDesc.h"
#include "../GameObjects/IGameObjectInstantiator.h"

// コンストラクタ
Components::Component::Component(const ComponentDesc& desc)
	: m_refOwner{ desc.refOwner }
	, m_refIGameObjectInstantiator{ desc.refIGameObjectInstantiator }
{
}

// ゲームオブジェクトを生成
Ref<GameObject> Components::Component::Instantiate(const std::string& name) const
{
	return m_refIGameObjectInstantiator->Instantiate(name);
}
