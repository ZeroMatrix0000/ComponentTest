/*
 * FileName:     GameObjectManager.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/23
 *
 * ゲームオブジェクト管理
 */

#include "Pch.h"
#include "GameObjectManager.h"

#include "GameObject.h"
#include "../Systems/IResources.h"
#include "../Systems/IErrorMessage.h"

// コンストラクタ
GameObjects::GameObjectManager::GameObjectManager(Ref<IComponentManager> iComponentManager, Ref<Systems::IResources> iResources)
	: IGameObjectManager{}
	, m_AddComponentList{}
	, m_nullReference{}
	, m_refGameObjects{ m_dontDestroyOnLoadGameObjects }
	, m_dontDestroyOnLoadGameObjects{}
	, m_refIComponentManager{ iComponentManager }
	, m_refIResources{ iResources }
{
	m_nullReference = std::make_unique<GameObject>(iComponentManager);
}

// 管理するゲームオブジェクトの参照を設定
void GameObjects::GameObjectManager::SetGameObjectsRef(Ref<std::vector<std::unique_ptr<GameObject>>> gameObjects)
{
	m_refGameObjects = gameObjects;
}

// ゲームオブジェクトを読み込む
void GameObjects::GameObjectManager::Load(const std::string& jsonName)
{
	// Json
	const auto* json = m_refIResources->GetJson(jsonName);

	m_refGameObjects->clear();

	// ゲームオブジェクトを生成
	for (const auto& element : *json)
	{
		m_refGameObjects->push_back(std::make_unique<GameObject>(m_refIComponentManager));
		// 名前を設定
		auto it = element.find("Name");
		if (it != element.end() && it.value().is_string())
		{
			m_refGameObjects->back()->SetName(it.value());
		}
	}

	int i = -1;
	// Prefab を読み込む
	for (const auto& element : *json)
	{
		i++;

		// Prefab名を探す
		auto it = element.find("Prefab");
		if (it == element.end() || !it.value().is_string())
		{
			continue;
		}
		// Json
		const auto* prefabJson = m_refIResources->GetJson(it.value().get<std::string>());

		// コンポーネントを追加
		std::vector<Component*> pComponents = AddComponents(*prefabJson, Ref{ *m_refGameObjects->at(i) });
		// コンポーネントを初期化
		InitializeComponents(*prefabJson, pComponents);
	}

	i = -1;
	// コンポーネントリスト
	std::vector<std::vector<Component*>> pComponentsList{};
	// コンポーネントを追加
	for (const auto& element : *json)
	{
		i++;

		// コンポーネントリストを探す
		auto it = element.find("Components");
		if (it == element.end() || !it.value().is_object())
		{
			pComponentsList.push_back(std::vector<Component*>{});
			continue;
		}

		// コンポーネントを追加
		pComponentsList.push_back(AddComponents(it.value(), Ref{ *m_refGameObjects->at(i) }));
	}

	i = -1;
	// コンポーネントを初期化
	for (const auto& element : *json)
	{
		i++;

		// コンポーネントリストを探す
		auto it = element.find("Components");
		if (it == element.end() || !it.value().is_object())
		{
			continue;
		}

		// コンポーネントを初期化
		InitializeComponents(it.value(), pComponentsList.at(i));
	}
}

// ゲームオブジェクトを名前で検索
Ref<GameObject> GameObjects::GameObjectManager::Find(const std::string& name) const
{
	auto it = std::ranges::find_if(m_refGameObjects.Get(), [&](const std::unique_ptr<GameObject>& gameObject) {return gameObject->GetName() == name; });
	if (it == m_refGameObjects->end())
	{
		// エラーメッセージを追加
		Systems::IErrorMessage::GetInstance()->AddMessage(Utility::FormatWString
		(
			L"ゲームオブジェクトが見つかりません。 | name: %s",
			Utility::string2wstring(name).c_str()
		));
		return Ref{ *m_nullReference };
	}
	else
	{
		return Ref{ *it->get() };
	}
}

// ゲームオブジェクトを生成
Ref<GameObject> GameObjects::GameObjectManager::Instantiate(const std::string& jsonName, bool dontDestroyOnLoad)
{
	// Json
	const auto* json = m_refIResources->GetJson(jsonName);

	// ゲームオブジェクト
	std::unique_ptr<GameObject> owner = std::make_unique<GameObject>(m_refIComponentManager);
	// ポインタ
	Ref<GameObject> refOwner{ *owner };
	refOwner->SetName(jsonName);
	// コンポーネントを追加
	std::vector<Component*> pComponents = AddComponents(*json, refOwner);
	// コンポーネントを初期化
	InitializeComponents(*json, pComponents);

	if (dontDestroyOnLoad)
	{
		m_dontDestroyOnLoadGameObjects.push_back(std::move(owner));
	}
	else
	{
		m_refGameObjects->push_back(std::move(owner));
	}

	return refOwner;
}

// コンポーネントを追加
std::vector<Component*> GameObjects::GameObjectManager::AddComponents(const nlohmann::ordered_json& json, Ref<GameObject> owner)
{
	std::vector<Component*> pComponents{};

	for (const auto& element : json.items())
	{
		// コンポーネント追加関数
		auto it = m_AddComponentList.find(element.key());
		if (it == m_AddComponentList.end())
		{
			// エラーメッセージを追加
			Systems::IErrorMessage::GetInstance()->AddMessage(Utility::FormatWString
			(
				L"不明なコンポーネントです。 | name: %s",
				Utility::string2wstring(element.key()).c_str()
			));
			pComponents.push_back(nullptr);
			continue;
		}

		// コンポーネントを追加
		pComponents.push_back(&it->second(owner).Get());
	}

	return pComponents;
}

// コンポーネントを初期化
void GameObjects::GameObjectManager::InitializeComponents(const nlohmann::ordered_json& json, const std::vector<Component*>& pComponents)
{
	int i = -1;
	for (const auto& element : json)
	{
		i++;

		if (pComponents.at(i) == nullptr)
		{
			continue;
		}

		pComponents.at(i)->Initalize(element, Ref<IGameObjectFinder>{ *this });
	}
}
