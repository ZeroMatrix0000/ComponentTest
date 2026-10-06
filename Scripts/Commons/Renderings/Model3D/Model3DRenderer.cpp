/*
 * FileName:     Model3DRenderer.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/06
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
	, m_effect{}
	, m_instanceBuffer{}
	, m_inputLayouts{}
	, m_pModels{}
	, m_pDevice{}
	, m_pContext{}
	, m_pCommonStates{}
{
}

// 初期化処理
void Renderings::Model3DRenderer::Initialize(ID3D11Device5* pDevice, ID3D11DeviceContext4* pContext, const DirectX::CommonStates& commonStates)
{
	m_pDevice = pDevice;
	m_pContext = pContext;
	m_pCommonStates = &commonStates;

	// 法線マップエフェクトを作成
	m_effect = std::make_unique<DirectX::NormalMapEffect>(pDevice);
	// インスタンシング有効化
	m_effect->SetInstancingEnabled(true);

	// バッファの詳細
	D3D11_BUFFER_DESC desc{};
	desc.ByteWidth = sizeof(DirectX::XMFLOAT3X4) * 1024;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	// インスタンスバッファを作成
	Utility::ThrowIfFailed(pDevice->CreateBuffer
	(
		&desc,
		nullptr,
		m_instanceBuffer.GetAddressOf()
	));
}

// 描画処理
void Renderings::Model3DRenderer::Render()
{
	for (const auto& pModels : m_pModels)
	{
		// 行列リスト
		std::vector<DirectX::XMFLOAT3X4> matrices{};

		for (const auto* pModel : pModels.second)
		{
			// モデルソース
			const Model3DSource* modelSource = pModels.first;

			// モデルの所有者のトランスフォーム
			const Transform* pTransform = pModel->GetConstPOwner()->GetConstComponent<Transform>();

			// カメラ画面
			for (const auto* pICameraScreen : pModel->GetPICameraScreens())
			{
				// 行列
				Math::Matrix matrix = pTransform->CreateWorldMatrix() * pICameraScreen->GetViewMatrix() * pICameraScreen->GetProjectionMatrix();
				// 行列リストに追加
				DirectX::XMStoreFloat3x4(&matrices.emplace_back(), matrix);

				//modelSource->GetModel().Draw
				//(
				//	m_pContext,
				//	*m_pCommonStates,
				//	pTransform->CreateWorldMatrix(),
				//	pICameraScreen->GetViewMatrix(),
				//	pICameraScreen->GetProjectionMatrix()
				//);
			}
		}

		// 行列リストが空なら何もしない
		if (matrices.empty())
		{
			continue;
		}

		// サブリソース
		D3D11_MAPPED_SUBRESOURCE mapped{};
		// GPU へ転送
		Utility::ThrowIfFailed(m_pContext->Map
		(
			m_instanceBuffer.Get(),
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&mapped
		));
		std::memcpy(mapped.pData, matrices.data(), sizeof(DirectX::XMFLOAT3X4) * matrices.size());
		m_pContext->Unmap(m_instanceBuffer.Get(), 0);

		m_effect->Apply(m_pContext);

		UINT instanceStride = sizeof(DirectX::XMFLOAT3X4);
		UINT instanceOffset = 0;
		m_pContext->IASetVertexBuffers
		(
			1,
			1,
			m_instanceBuffer.GetAddressOf(),
			&instanceStride,
			&instanceOffset
		);

		// モデル
		const auto& pModel = pModels.first->GetModel();

		for (size_t meshIndex = 0; meshIndex < pModel.meshes.size(); meshIndex++)
		{
			// メッシュ
			const auto& mesh = pModel.meshes.at(meshIndex);

			for (size_t partIndex = 0; partIndex < mesh->meshParts.size(); partIndex++)
			{
				auto* pInputLayouts = m_inputLayouts.at(pModels.first).at(meshIndex).at(partIndex).Get();

				m_pContext->IASetInputLayout(pInputLayouts);

				mesh->meshParts.at(partIndex)->DrawInstanced
				(
					m_pContext,
					m_effect.get(),
					pInputLayouts,
					static_cast<uint32_t>(matrices.size())
				);
			}
		}
	}
}

// モデルのポインタを追加
void Renderings::Model3DRenderer::AddPModel(const Model3D* pModel)
{
	// モデルソース
	const Model3DSource* pModelSource = pModel->GetPModelSource();
	if (!pModelSource)
	{
		return;
	}

	// モデルソースに対応したモデルのポインタリストが見つからなければ生成
	auto it = m_pModels.find(pModelSource);
	if (it == m_pModels.end())
	{
		m_pModels.emplace(pModelSource, std::vector<const Model3D*>{});
		CreateInputLayout(pModelSource);
	}

	// モデルのポインタの追加
	m_pModels.at(pModelSource).push_back(pModel);
}

// モデルのポインタを削除
void Renderings::Model3DRenderer::RemovePModel(const Model3D* pModel)
{
	// モデルソース
	const Model3DSource* pModelSource = pModel->GetPModelSource();
	if (!pModelSource)
	{
		return;
	}

	// ポインタリスト
	auto& pModels = m_pModels.at(pModelSource);

	auto it = std::ranges::find(pModels, pModel);
	if (it != pModels.end())
	{
		pModels.erase(it);
	}
}

// 入力レイアウトを作成
void Renderings::Model3DRenderer::CreateInputLayout(const Model3DSource* pModelSource)
{
	m_inputLayouts.emplace(pModelSource, std::vector<std::vector<Microsoft::WRL::ComPtr<ID3D11InputLayout>>>{});
	// 入力レイアウトリスト
	auto& inputLayouts = m_inputLayouts.at(pModelSource);

	for (const auto& mesh : pModelSource->GetModel().meshes)
	{
		inputLayouts.push_back(std::vector<Microsoft::WRL::ComPtr<ID3D11InputLayout>>{});
		for (const auto& part : mesh->meshParts)
		{
			// 入力レイアウトの詳細
			std::vector<D3D11_INPUT_ELEMENT_DESC> elements;

			// 頂点データの追加
			for (const auto& element : *(part->vbDecl))
			{
				elements.push_back(element);
			}

			// 行列データの追加
			elements.push_back(
			{
				"InstMatrix",
				0,
				DXGI_FORMAT_R32G32B32A32_FLOAT,
				1,
				0,
				D3D11_INPUT_PER_INSTANCE_DATA,
				1
			});
			elements.push_back(
			{
				"InstMatrix",
				1,
				DXGI_FORMAT_R32G32B32A32_FLOAT,
				1,
				16,
				D3D11_INPUT_PER_INSTANCE_DATA,
				1
			});
			elements.push_back(
			{
				"InstMatrix",
				2,
				DXGI_FORMAT_R32G32B32A32_FLOAT,
				1,
				32,
				D3D11_INPUT_PER_INSTANCE_DATA,
				1
			});

			const void* byteCode = nullptr;
			size_t byteCodeSize = 0;

			m_effect->GetVertexShaderBytecode(&byteCode, &byteCodeSize);

			Utility::ThrowIfFailed(m_pDevice->CreateInputLayout
			(
				elements.data(),
				static_cast<UINT>(elements.size()),
				byteCode,
				byteCodeSize,
				inputLayouts.back().emplace_back().GetAddressOf()
			));
		}
	}
}
