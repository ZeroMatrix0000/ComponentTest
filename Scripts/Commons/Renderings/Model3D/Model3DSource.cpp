/*
 * FileName:     Model3DSource.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/08
 *
 * 3Dモデルのソース
 */

#include "Pch.h"
#include "Model3DSource.h"

#include "InstancingEffectFactory.h"

// コンストラクタ
Renderings::Model3DSource::Model3DSource()
	: m_model{}
{
}

// 初期化処理
void Renderings::Model3DSource::Initialize(ID3D11Device* device, InstancingEffectFactory* fx, const std::wstring& filePath)
{
	fx->SetModel3DSource(this);
	m_model = DirectX::Model::CreateFromCMO(device, filePath.c_str(), *fx);
}

// パーツごとの情報を追加
void Renderings::Model3DSource::AddPartInfo
(
	const DirectX::IEffectFactory::EffectInfo& effectInfo,
	ID3D11ShaderResourceView* pTexture,
	ID3D11ShaderResourceView* pNormalMap,
	ID3D11ShaderResourceView* pSpecularMap
)
{
	auto& partInfo = m_partInfos.emplace_back();
	partInfo.effectInfo = effectInfo;
	partInfo.texture.Attach(pTexture);
	partInfo.normalMap.Attach(pNormalMap);
	partInfo.specularMap.Attach(pSpecularMap);
}

// 生成
Renderings::Model3DSource Renderings::Model3DSource::Create(ID3D11Device* device, InstancingEffectFactory* fx, const std::wstring& filePath)
{
	Model3DSource model;
	model.Initialize(device, fx, filePath);
	return model;
}
