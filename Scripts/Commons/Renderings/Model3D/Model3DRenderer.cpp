/*
 * FileName:     Model3DRenderer.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/07
 *
 * 3Dモデル描画
 */

#include "Pch.h"
#include "Model3DRenderer.h"

#include "Model3D.h"
#include "Model3DSource.h"
#include "../ICameraScreen.h"
#include "../VertexShader.h"
#include "Scripts/Commons/GameObjects/GameObject.h"
#include "Scripts/Commons/Components/Transform.h"
#include "Scripts/Commons/Systems/IResources.h"

 // コンストラクタ
Renderings::Model3DRenderer::Model3DRenderer()
	: IModel3DRenderer{}
	, m_instanceBuffer{}
	, m_inputLayout{}
	, m_pModels{}
	, m_pDevice{}
	, m_pContext{}
	, m_pCommonStates{}
	, m_pInstancingVS{}
{
}

// 初期化処理
void Renderings::Model3DRenderer::Initialize(ID3D11Device5* pDevice, ID3D11DeviceContext4* pContext, const DirectX::CommonStates& commonStates)
{
	m_pDevice = pDevice;
	m_pContext = pContext;
	m_pCommonStates = &commonStates;

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
void Renderings::Model3DRenderer::Render(bool isInstance)
{
	// ピクセルシェーダ
	auto sampler = m_pCommonStates->LinearWrap();
	m_pContext->PSSetSamplers(0, 1, &sampler);

	m_pContext->OMSetDepthStencilState(m_pCommonStates->DepthDefault(), 0);

	for (const auto& [pICameraScreen, pModelsList] : m_pModels)
	{
		for (const auto& [pModelSource, pModels] : pModelsList)
		{
			// 行列リスト
			std::vector<DirectX::XMFLOAT3X4> matrices{};
			matrices.reserve(pModels.size() * 100);

			for (const auto* pModel : pModels)
			{
				// モデルの所有者のトランスフォーム
				const Transform* pTransform = pModel->GetConstPOwner()->GetConstComponent<Transform>();

				for (int i = 0; i < 100; i++)
				{
					if (isInstance)
					{
						// 行列リストに追加
						DirectX::XMStoreFloat3x4(&matrices.emplace_back(), pTransform->GetWorldMatrix());
					}
					else
					{
						pModelSource->GetModel().Draw
						(
							m_pContext,
							*m_pCommonStates,
							pTransform->GetWorldMatrix(),
							pICameraScreen->GetViewMatrix(),
							pICameraScreen->GetProjectionMatrix()
						);
					}
				}
			}

			// 行列リストが空なら何もしない
			if (matrices.empty())
			{
				continue;
			}

			UpdateInstanceBuffer(matrices);

			// モデル
			const auto& dxModel = pModelSource->GetModel();

			for (const auto& mesh : dxModel.meshes)
			{
				for (const auto& part : mesh->meshParts)
				{
					DrawInstanced(pICameraScreen, part.get(), matrices.size());
				}
			}
		}
	}
}

// インスタンシング頂点シェーダを設定
void Renderings::Model3DRenderer::SetInstancingVS(const VertexShader* pVertexShader)
{
	m_pInstancingVS = pVertexShader;
	CreateInputLayout();
}

// モデルのポインタを追加
void Renderings::Model3DRenderer::AddPModel(const ICameraScreen* pICameraScreen, const Model3D* pModel)
{
	if (!pICameraScreen)
	{
		return;
	}

	// モデルソース
	const Model3DSource* pModelSource = pModel->GetPModelSource();
	if (!pModelSource)
	{
		return;
	}

	// カメラに対応したモデルのポインタリストが見つからなければ生成
	auto cameraIt = m_pModels.find(pICameraScreen);
	if (cameraIt == m_pModels.end())
	{
		m_pModels.emplace(pICameraScreen, std::unordered_map<const Model3DSource*, std::vector<const Model3D*>>{});
		cameraIt = m_pModels.find(pICameraScreen);
	}

	// モデルソースに対応したモデルのポインタリストが見つからなければ生成
	auto modelIt = cameraIt->second.find(pModelSource);
	if (modelIt == cameraIt->second.end())
	{
		cameraIt->second.emplace(pModelSource, std::vector<const Model3D*>{});
		modelIt = cameraIt->second.find(pModelSource);
	}

	// モデルのポインタの追加
	modelIt->second.push_back(pModel);
}

// モデルのポインタを削除
void Renderings::Model3DRenderer::RemovePModel(const ICameraScreen* pICameraScreen, const Model3D* pModel)
{
	if (!pICameraScreen)
	{
		return;
	}

	// モデルソース
	const Model3DSource* pModelSource = pModel->GetPModelSource();
	if (!pModelSource)
	{
		return;
	}

	// ポインタリスト
	auto& pModels = m_pModels.at(pICameraScreen).at(pModelSource);

	auto it = std::ranges::find(pModels, pModel);
	if (it != pModels.end())
	{
		pModels.erase(it);
	}

	// 空の配列を削除
	if (pModels.size() == 0)
	{
		m_pModels.at(pICameraScreen).erase(pModelSource);
		if (m_pModels.at(pICameraScreen).size() == 0)
		{
			m_pModels.erase(pICameraScreen);
		}
	}
}

// 入力レイアウトを作成
void Renderings::Model3DRenderer::CreateInputLayout()
{
	const D3D11_INPUT_ELEMENT_DESC inputElements[] =
	{
		{
			"SV_Position",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			0,
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		},
		{
			"NORMAL",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			12,
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		},
		{
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			24,
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		},
		{
			"InstMatrix",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			1,
			0,
			D3D11_INPUT_PER_INSTANCE_DATA,
			1
		},
		{
			"InstMatrix",
			1,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			1,
			16,
			D3D11_INPUT_PER_INSTANCE_DATA,
			1
		},
		{
			"InstMatrix",
			2,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			1,
			32,
			D3D11_INPUT_PER_INSTANCE_DATA,
			1
		}
	};

	Utility::ThrowIfFailed(m_pDevice->CreateInputLayout
	(
		inputElements,
		_countof(inputElements),
		m_pInstancingVS->GetBlob()->GetBufferPointer(),
		m_pInstancingVS->GetBlob()->GetBufferSize(),
		m_inputLayout.GetAddressOf()
	));
}

// インスタンスバッファを更新
void Renderings::Model3DRenderer::UpdateInstanceBuffer(const std::vector<DirectX::XMFLOAT3X4>& matrices)
{
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
}

// インスタンス描画
void Renderings::Model3DRenderer::DrawInstanced(const ICameraScreen* pICameraScreen, const DirectX::ModelMeshPart* pPart, size_t matricesCount)
{
	// エフェクト
	auto* effect = static_cast<DirectX::BasicEffect*>(pPart->effect.get());
	effect->SetWorld(Math::Matrix::Identity);
	effect->SetView(pICameraScreen->GetViewMatrix());
	effect->SetProjection(pICameraScreen->GetProjectionMatrix());
	effect->Apply(m_pContext);

	// 頂点シェーダ
	m_pContext->VSSetShader(m_pInstancingVS->GetD3DShader(), nullptr, 0);

	// 入力レイアウト
	m_pContext->IASetInputLayout(m_inputLayout.Get());

	// 頂点バッファ
	ID3D11Buffer* buffers[] =
	{
		pPart->vertexBuffer.Get(),
		m_instanceBuffer.Get()
	};
	UINT strides[] =
	{
		pPart->vertexStride,
		sizeof(DirectX::XMFLOAT3X4)
	};
	UINT offsets[] =
	{
		0,
		0
	};
	m_pContext->IASetVertexBuffers(
		0,
		2,
		buffers,
		strides,
		offsets
	);

	// 番号バッファ
	m_pContext->IASetIndexBuffer
	(
		pPart->indexBuffer.Get(),
		pPart->indexFormat,
		0
	);

	// トポロジー
	m_pContext->IASetPrimitiveTopology(pPart->primitiveType);

	// 描画
	m_pContext->DrawIndexedInstanced(
		pPart->indexCount,
		static_cast<UINT>(matricesCount),
		pPart->startIndex,
		pPart->vertexOffset,
		0
	);
}
