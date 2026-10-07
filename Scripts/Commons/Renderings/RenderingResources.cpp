/*
 * FileName:     RenderingResources.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/07
 *
 * 描画リソース
 */

#include "Pch.h"
#include "RenderingResources.h"

 // コンストラクタ
Renderings::RenderingResources::RenderingResources()
	: OnlyOne{ typeid(RenderingResources) }
	, m_commonStates{}
	, m_effectFactory{}
{
}

// 初期化処理
void Renderings::RenderingResources::Initialize(ID3D11Device5* device)
{
	m_commonStates = std::make_unique<DirectX::CommonStates>(device);
	m_effectFactory = std::make_unique<InstancingEffectFactory>(device);
}

// エフェクトファクトリーのディレクトリを設定
void Renderings::RenderingResources::SetEffectFactoryDirectory(const std::wstring& filePath)
{
	m_effectFactory->SetDirectory(filePath.c_str());
}
