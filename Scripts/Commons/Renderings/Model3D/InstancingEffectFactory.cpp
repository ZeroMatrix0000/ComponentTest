/*
 * FileName:     InstancingEffectFactory.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/07
 *
 * インスタンシング描画用のエフェクトファクトリ
 */

#include "Pch.h"
#include "InstancingEffectFactory.h"

// コンストラクタ
Renderings::InstancingEffectFactory::InstancingEffectFactory(ID3D11Device* pDevice)
	: m_effects{}
	, m_directory{}
	, m_pDevice{ pDevice }
{
}

// エフェクトを作成
std::shared_ptr<DirectX::IEffect> Renderings::InstancingEffectFactory::CreateEffect(const EffectInfo& info, ID3D11DeviceContext* pContext)
{
	auto& effect = m_effects.emplace_back(std::make_shared<DirectX::BasicEffect>(m_pDevice));

	// 色
	effect->SetDiffuseColor(DirectX::XMLoadFloat3(&info.diffuseColor));
	effect->SetEmissiveColor(DirectX::XMLoadFloat3(&info.emissiveColor));
	effect->SetSpecularColor(DirectX::XMLoadFloat3(&info.specularColor));
	effect->SetSpecularPower(info.specularPower);
	effect->SetAlpha(info.alpha);

	// 光
	effect->SetAmbientLightColor(Math::Color{ 0.2f, 0.2f, 0.2f, 1.0f });
	effect->SetLightEnabled(0, true);
	effect->SetLightDirection(0, Math::Vector3{ 0.0f, -1.0f, 0.0f });
	effect->SetLightEnabled(1, true);
	effect->SetLightDirection(1, Math::Vector3{ 0.0f, 0.0f, -0.5f });
	effect->SetLightEnabled(2, true);
	effect->SetLightDirection(2, Math::Vector3{ -0.25f, 0.0f, 0.25f });
	effect->SetLightingEnabled(true);

	// テクスチャ
	if (info.diffuseTexture && !std::wstring{ info.diffuseTexture }.empty())
	{
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;

		CreateTexture
		(
			info.diffuseTexture,
			pContext,
			texture.GetAddressOf()
		);
		effect->SetTexture(texture.Get());
	}

	return effect;
}

// テクスチャを作成
void Renderings::InstancingEffectFactory::CreateTexture(const wchar_t* name, ID3D11DeviceContext* pContext, ID3D11ShaderResourceView** ppTextureView)
{
	if (!name || !ppTextureView)
	{
		return;
	}

	*ppTextureView = nullptr;

	std::wstring path = m_directory + name;

	if (std::filesystem::path{ path }.extension() == L".dds")
	{
		Utility::ThrowIfFailed(DirectX::CreateDDSTextureFromFile
		(
			m_pDevice,
			path.c_str(),
			nullptr,
			ppTextureView
		));
	}
	else
	{
		Utility::ThrowIfFailed(DirectX::CreateWICTextureFromFile
		(
			m_pDevice,
			pContext,
			path.c_str(),
			nullptr,
			ppTextureView
		));
	}
}
