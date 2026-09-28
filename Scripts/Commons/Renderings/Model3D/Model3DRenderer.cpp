/*
 * FileName:     Model3DRenderer.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/09/28
 *
 * 3Dモデル描画
 */

#include "Pch.h"
#include "Model3DRenderer.h"

#include "Model3D.h"
#include "Model3DSource.h"
#include "../ICameraScreen.h"
#include "Scripts/Commons/GameObjects/GameObject.h"
#include "Scripts/Commons/Components/Transform.h"
#include "Scripts/Commons/Systems/IResources.h"

 // コンストラクタ
Renderings::Model3DRenderer::Model3DRenderer()
	: IModel3DRenderer{}
	, m_pModels{}
	, m_pContext{}
	, m_pCommonStates{}
{
}

// 初期化処理
void Renderings::Model3DRenderer::Initialize(ID3D11DeviceContext4* pContext, const DirectX::CommonStates& commonStates)
{
	m_pContext = pContext;
	m_pCommonStates = &commonStates;
}

// 描画処理
void Renderings::Model3DRenderer::Render()
{
	for (const auto* pModel : m_pModels)
	{
		// モデルソース
		const Model3DSource* modelSource = pModel->GetPModelSource();
		if (!modelSource)
		{
			continue;
		}

		// モデルの所有者のトランスフォーム
		const Transform* pTransform = pModel->GetConstPOwner()->GetConstComponent<Transform>();

		// カメラ画面
		for (const auto* pICameraScreen : pModel->GetPICameraScreens())
		{
			modelSource->GetModel().Draw
			(
				m_pContext,
				*m_pCommonStates,
				pTransform->CreateWorldMatrix(),
				pICameraScreen->GetViewMatrix(),
				pICameraScreen->GetProjectionMatrix()
			);
		}
	}
}

// モデルのポインタを追加
void Renderings::Model3DRenderer::AddPModel(const Model3D* pModel)
{
	m_pModels.push_back(pModel);
}

// モデルのポインタを削除
void Renderings::Model3DRenderer::RemovePModel(const Model3D* pModel)
{
	auto it = std::ranges::find(m_pModels, pModel);
	if (it != m_pModels.end())
	{
		m_pModels.erase(it);
	}
}
