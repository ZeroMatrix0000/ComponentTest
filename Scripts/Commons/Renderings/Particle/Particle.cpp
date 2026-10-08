/*
 * FileName:     Particle.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/08
 *
 * 3Dモデル
 */

#include "Pch.h"
#include "Particle.h"

#include "../Model3D/IModel3DRenderer.h"
#include "Scripts/Commons/GameObjects/GameObject.h"
#include "Scripts/Commons/GameObjects/IGameObjectFinder.h"
#include "Scripts/Commons/Renderings/CameraScreen.h"
#include "Scripts/Commons/Systems/IResources.h"
#include "Scripts/Commons/Systems/JsonSerializer.h"

Renderings::Particle::Particle(const ComponentDesc& desc, IModel3DRenderer* pIModelRenderer, const Systems::IResources& iResources)
	: Component{ desc }
	, m_pModelSource{}
	, m_pICameraScreens{}
	, m_pIModelRenderer{ pIModelRenderer }
	, m_refIResources{ iResources }
{
}

// デストラクタ
Renderings::Particle::~Particle()
{
	// 描画者からモデルを削除
	for (const auto* pICameraScreen : m_pICameraScreens)
	{
		m_pIModelRenderer->RemovePParticle(pICameraScreen, this);
	}
}

// 初期化処理
void Renderings::Particle::Initalize(const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder)
{
	std::string modelSourceName{};

	Systems::JsonSerializer serializer{ pIGameObjectFinder };
	serializer.AddParameter(&modelSourceName, "ModelSourceName");
	serializer.AddParameter(&m_pICameraScreens, "CameraScreens");
	serializer.Load(json);

	if (!modelSourceName.empty())
	{
		SetModelSource(modelSourceName);
	}
}

// モデルソースを設定
void Renderings::Particle::SetModelSource(const std::string& modelSourceName)
{
	// 描画者からモデルを削除
	for (const auto* pICameraScreen : m_pICameraScreens)
	{
		m_pIModelRenderer->RemovePParticle(pICameraScreen, this);
	}
	m_pModelSource = m_refIResources.GetModelSource(modelSourceName);
	// 描画者にモデルを追加
	for (const auto* pICameraScreen : m_pICameraScreens)
	{
		m_pIModelRenderer->AddPParticle(pICameraScreen, this);
	}
}

// 映るカメラ画面を追加
void Renderings::Particle::AddICameraScreen(const ICameraScreen& iCameraScreen)
{
	if (std::ranges::find(m_pICameraScreens, &iCameraScreen) == m_pICameraScreens.end())
	{
		m_pICameraScreens.push_back(&iCameraScreen);
		m_pIModelRenderer->AddPParticle(&iCameraScreen, this);
	}
}

// 映るカメラ画面を削除
void Renderings::Particle::RemoveICameraScreen(const ICameraScreen& iCameraScreen)
{
	auto it = std::ranges::find(m_pICameraScreens, &iCameraScreen);
	if (it != m_pICameraScreens.end())
	{
		m_pICameraScreens.erase(it);
		m_pIModelRenderer->RemovePParticle(&iCameraScreen, this);
	}
}
