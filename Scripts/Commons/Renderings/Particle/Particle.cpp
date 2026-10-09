/*
 * FileName:     Particle.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/09
 *
 * パーティクル
 */

#include "Pch.h"
#include "Particle.h"

#include "../Model3D/IModel3DRenderer.h"
#include "Scripts/Commons/GameObjects/GameObject.h"
#include "Scripts/Commons/GameObjects/IGameObjectFinder.h"
#include "Scripts/Commons/Renderings/CameraScreen.h"
#include "Scripts/Commons/Systems/JsonSerializer.h"
#include "Scripts/Commons/Systems/IResources.h"

// コンストラクタ
Renderings::Particle::Particle(IModel3DRenderer* pIModelRenderer, const Systems::IResources& iResources)
	: m_updateElements{}
	, m_renderElements{}
	, m_isActive{ true }
	, m_pICameraScreens{}
	, m_pIModelRenderer{ pIModelRenderer }
	, m_refIResources{ iResources }
{
}

// デストラクタ
Renderings::Particle::~Particle()
{
	// 描画者からパーティクルを削除
	for (const auto* pICameraScreen : m_pICameraScreens)
	{
		for (const auto& renderElement : m_renderElements)
		{
			m_pIModelRenderer->RemovePParticle(pICameraScreen, renderElement.get());
		}
	}
}

// ワールド行列を適用
void Renderings::Particle::ApplyTransform()
{
	for (int i = 0; i < m_updateElements.size(); i++)
	{
		// 更新情報
		UpdateElement& updateElement = m_updateElements.at(i);
		
		// 描画情報を更新
		m_renderElements.at(i)->matrix = 
			Math::Matrix::CreateScale(updateElement.scale) *
			Math::Matrix::CreateFromQuaternion(updateElement.rotation) *
			Math::Matrix::CreateTranslation(updateElement.position)
		;
	}
}

// モデルソースを設定
void Renderings::Particle::SetModelSource(const std::string& modelSourceName, RenderElement* pRenderElement)
{
	// 描画者からパーティクルを削除
	for (const auto* pICameraScreen : m_pICameraScreens)
	{
		m_pIModelRenderer->RemovePParticle(pICameraScreen, pRenderElement);
	}
	pRenderElement->pModelSource = m_refIResources.GetModelSource(modelSourceName);
	// 描画者にパーティクルを追加
	for (const auto* pICameraScreen : m_pICameraScreens)
	{
		m_pIModelRenderer->AddPParticle(pICameraScreen, pRenderElement);
	}
}

// 映るカメラ画面を追加
void Renderings::Particle::AddICameraScreen(const ICameraScreen& iCameraScreen)
{
	if (std::ranges::find(m_pICameraScreens, &iCameraScreen) == m_pICameraScreens.end())
	{
		m_pICameraScreens.push_back(&iCameraScreen);
		for (const auto& renderElement : m_renderElements)
		{
			m_pIModelRenderer->AddPParticle(&iCameraScreen, renderElement.get());
		}
	}
}

// 映るカメラ画面を削除
void Renderings::Particle::RemoveICameraScreen(const ICameraScreen& iCameraScreen)
{
	auto it = std::ranges::find(m_pICameraScreens, &iCameraScreen);
	if (it != m_pICameraScreens.end())
	{
		m_pICameraScreens.erase(it);
		for (const auto& renderElement : m_renderElements)
		{
			m_pIModelRenderer->RemovePParticle(&iCameraScreen, renderElement.get());
		}
	}
}

// 要素を追加
void Renderings::Particle::AddElement(UpdateElement** ppUpdateElement, RenderElement** ppRenderElement)
{
	*ppUpdateElement = &m_updateElements.emplace_back();

	auto& renderElement = m_renderElements.emplace_back();
	renderElement = std::make_unique<RenderElement>();
	*ppRenderElement = renderElement.get();
}
