/*
 * FileName:     ComponentManager.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/22
 *
 * コンポーネント管理
 */

#include "Pch.h"
#include "ComponentManager.h"

#include "../Systems/IErrorMessage.h"
#include "ComponentDesc.h"
#include "../GameObjects/IGameObjectInstantiator.h"

// コンストラクタ
Components::ComponentManager::ComponentManager(Ref<IGameObjectInstantiator> iGameObjectInstantiator)
	: IComponentManager{}
	, m_permit{}
	, m_CreateComponentList{}
	, m_nullReferences{}
	, m_refIGameObjectInstantiator{ iGameObjectInstantiator }
{
}

// コンポーネントを作成
std::unique_ptr<Component> Components::ComponentManager::Create(const std::type_index& index, GameObject* pOwner)
{
	return m_CreateComponentList.at(index)(ComponentDesc{ pOwner, m_refIGameObjectInstantiator, m_permit });
}

// コンポーネント作成関数を追加
void Components::ComponentManager::RegisterCreate
(
	const std::type_index& index,
	const std::function<std::unique_ptr<Component>(const ComponentDesc&)>& CreateComponent
)
{
	if (m_CreateComponentList.find(index) == m_CreateComponentList.end())
	{
		m_CreateComponentList.emplace(index, CreateComponent);
	}
}

// 未参照コンポーネントを取得
Component& Components::ComponentManager::GetNullReferences(const std::type_index& index, bool addErrorMessage)
{
	if (addErrorMessage)
	{
		// エラーメッセージを追加
		Systems::IErrorMessage::GetInstance()->AddMessage(Utility::FormatWString
		(
			L"コンポーネントが見つかりません。 | %s",
			Utility::string2wstring(index.name()).c_str()
		));
	}

	auto it = m_nullReferences.find(index);
	if (it == m_nullReferences.end())
	{
		// コンポーネントを追加
		m_nullReferences.emplace(index, m_CreateComponentList.at(index)(ComponentDesc
		{
			Ref{ m_refIGameObjectInstantiator->GetNullReferences() },
			Ref{ m_refIGameObjectInstantiator },
			Ref{ m_permit }.Const()
		}));
		return *m_nullReferences.at(index);
	}
	else
	{
		return *it->second;
	}
}
