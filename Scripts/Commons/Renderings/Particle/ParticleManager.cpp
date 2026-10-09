/*
 * FileName:     ParticleManager.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/09
 *
 * パーティクル管理
 */

#include "Pch.h"
#include "ParticleManager.h"

#include "../Model3D/IModel3DRenderer.h"
#include "Scripts/Commons/Components/Transform.h"
#include "Scripts/Commons/GameObjects/GameObject.h"
#include "Scripts/Commons/GameObjects/IGameObjectFinder.h"
#include "Scripts/Commons/Renderings/CameraScreen.h"
#include "Scripts/Commons/Systems/IResources.h"
#include "Scripts/Commons/Systems/JsonSerializer.h"

Renderings::ParticleManager::ParticleManager(const ComponentDesc& desc, IModel3DRenderer* pIModelRenderer, const Systems::IResources& iResources)
	: Component{ desc }
	, m_particles{}
	, m_pTransform{ GetPOwner()->GetNullReferences<Transform>() }
	, m_pICameraScreens{}
	, m_pIModelRenderer{ pIModelRenderer }
	, m_refIResources{ iResources }
{
}

// 初期化処理
void Renderings::ParticleManager::Initalize(const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder)
{
	m_pTransform = GetPOwner()->GetComponent<Transform>();

	Systems::JsonSerializer serializer{ pIGameObjectFinder };
	serializer.AddParameter(&m_pICameraScreens, "CameraScreens");
	serializer.Load(json);
}

// 更新処理
void Renderings::ParticleManager::Update(float elapsedTime)
{
	// パーティクルを更新
	for (auto& particle : m_particles)
	{
		particle->Update(elapsedTime);
	}

	// アクティブでないパーティクルを削除
	for (int i = 0; i < m_particles.size();)
	{
		if (!m_particles.at(i)->IsActive())
		{
			m_particles.erase(m_particles.begin() + i);
		}
		else
		{
			i++;
		}
	}
}

// 映るカメラ画面を追加
void Renderings::ParticleManager::AddICameraScreen(const ICameraScreen& iCameraScreen)
{
	if (std::ranges::find(m_pICameraScreens, &iCameraScreen) == m_pICameraScreens.end())
	{
		m_pICameraScreens.push_back(&iCameraScreen);
		for (auto& particle : m_particles)
		{
			particle->AddICameraScreen(iCameraScreen);
		}
	}
}

// 映るカメラ画面を削除
void Renderings::ParticleManager::RemoveICameraScreen(const ICameraScreen& iCameraScreen)
{
	auto it = std::ranges::find(m_pICameraScreens, &iCameraScreen);
	if (it != m_pICameraScreens.end())
	{
		m_pICameraScreens.erase(it);
		for (auto& particle : m_particles)
		{
			particle->RemoveICameraScreen(iCameraScreen);
		}
	}
}
