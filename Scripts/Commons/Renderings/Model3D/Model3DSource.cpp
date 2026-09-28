/*
 * FileName:     Model3DSource.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/07/01
 *
 * 3Dモデルのソース
 */

#include "Pch.h"
#include "Model3DSource.h"

// コンストラクタ
Renderings::Model3DSource::Model3DSource()
	: m_model{}
{
}

// 初期化処理
void Renderings::Model3DSource::Initialize(ID3D11Device* device, DirectX::EffectFactory* fx, const std::wstring& filePath)
{
	m_model = DirectX::Model::CreateFromCMO(device, filePath.c_str(), *fx);
}

// 生成
Renderings::Model3DSource Renderings::Model3DSource::Create(ID3D11Device* device, DirectX::EffectFactory* fx, const std::wstring& filePath)
{
	Model3DSource model;
	model.Initialize(device, fx, filePath);
	return model;
}
