/*
 * FileName:     Model3DRenderer.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/08
 *
 * 3Dモデル描画
 */

#include "Pch.h"
#include "Model3DRenderer.h"

#include "Model3D.h"
#include "Model3DSource.h"
#include "../ICameraScreen.h"
#include "../Particle/Particle.h"
#include "../Shader/VertexShader.h"
#include "../Shader/PixelShader.h"
#include "Scripts/Commons/GameObjects/GameObject.h"
#include "Scripts/Commons/Components/Transform.h"
#include "Scripts/Commons/Systems/IResources.h"

 // コンストラクタ
Renderings::Model3DRenderer::Model3DRenderer()
	: IModel3DRenderer{}
	, m_instanceBuffer{}
	, m_inputLayout{}
	, m_renderObjects{}
	, m_pDevice{}
	, m_pContext{}
	, m_pCommonStates{}
	, m_pVertexShader{}
	, m_pPixelShader{}
	, m_whiteTexture{}
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
	desc.ByteWidth = sizeof(DirectX::XMFLOAT3X4) * 65536;
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

	CreateWhiteTexture();
}

// 描画処理
void Renderings::Model3DRenderer::Render(bool isInstance)
{
	// シェーダを設定
	m_pContext->VSSetShader(m_pVertexShader->GetD3DShader(), nullptr, 0);
	m_pContext->PSSetShader(m_pPixelShader->GetD3DShader(), nullptr, 0);

	// サンプラーを設定
	auto sampler = m_pCommonStates->LinearWrap();
	m_pContext->PSSetSamplers(0, 1, &sampler);

	// 深度ステンシルを設定
	m_pContext->OMSetDepthStencilState(m_pCommonStates->DepthDefault(), 0);

	// 定数バッファ
	ConstantBuffer* pVSConstantBuffer = m_pVertexShader->GetConstantBuffer();
	// 定数バッファ
	ConstantBuffer* pPSConstantBuffer = m_pPixelShader->GetConstantBuffer();

	for (const auto& [pICameraScreen, renderObjectsList] : m_renderObjects)
	{
		// 定数バッファを設定
		pVSConstantBuffer->SetVariable("View", pICameraScreen->GetViewMatrix().Transpose());
		pVSConstantBuffer->SetVariable("Projection", pICameraScreen->GetProjectionMatrix().Transpose());
		m_pVertexShader->SetConstantBuffer(m_pDevice, m_pContext);
		pPSConstantBuffer->SetVariable("AmbientColor", Math::Vector3{ 0.5f, 0.5f, 0.5f });
		pPSConstantBuffer->SetVariable("LightDirection", Math::Vector3{ -0.25f, -1.0f, -0.5f });
		pPSConstantBuffer->SetVariable("LightColor", Math::Vector3{ 0.5f, 0.5f, 0.5f });
		pPSConstantBuffer->SetVariable("EyePosition", pICameraScreen->GetEyePosition());
		m_pPixelShader->SetConstantBuffer(m_pDevice, m_pContext);

		for (const auto& [pModelSource, renderObjects] : renderObjectsList)
		{
			// 行列リスト
			std::vector<DirectX::XMFLOAT3X4> instMatrices{};
			instMatrices.reserve(renderObjects.pModels.size() + renderObjects.pParticleElements.size());

			for (const auto* pModel : renderObjects.pModels)
			{
				// モデルの所有者のトランスフォーム
				const Transform* pTransform = pModel->GetConstPOwner()->GetConstComponent<Transform>();

				// 行列リストに追加
				DirectX::XMStoreFloat3x4(&instMatrices.emplace_back(), pTransform->GetWorldMatrix());

				// インスタンス描画じゃないなら
				if (!isInstance)
				{
					// バッファを更新
					UpdateInstanceBuffer(instMatrices);

					// パーツ番号
					size_t partIndex = 0;
					for (const auto& mesh : pModelSource->GetModel().meshes)
					{
						for (const auto& part : mesh->meshParts)
						{
							// パーツの情報を設定
							SetPartInfo(pModelSource->GetPartInfo(partIndex));
							// 描画
							Draw(pICameraScreen, part.get());

							partIndex++;
						}
					}

					// マトリクスを削除
					instMatrices.clear();
				}
			}

			for (const auto* pElement : renderObjects.pParticleElements)
			{
				// 行列リストに追加
				DirectX::XMStoreFloat3x4(&instMatrices.emplace_back(), pElement->matrix);

				// インスタンス描画じゃないなら
				if (!isInstance)
				{
					// バッファを更新
					UpdateInstanceBuffer(instMatrices);

					// パーツ番号
					size_t partIndex = 0;
					for (const auto& mesh : pModelSource->GetModel().meshes)
					{
						for (const auto& part : mesh->meshParts)
						{
							// パーツの情報を設定
							SetPartInfo(pModelSource->GetPartInfo(partIndex));
							// 描画
							Draw(pICameraScreen, part.get());

							partIndex++;
						}
					}

					// マトリクスを削除
					instMatrices.clear();
				}
			}

			// インスタンスバッファを更新
			UpdateInstanceBuffer(instMatrices);

			// パーツ番号
			size_t partIndex = 0;
			for (const auto& mesh : pModelSource->GetModel().meshes)
			{
				for (const auto& part : mesh->meshParts)
				{
					// 定数バッファを設定
					SetPartInfo(pModelSource->GetPartInfo(partIndex));
					// 描画
					DrawInstanced(pICameraScreen, part.get(), instMatrices.size());

					partIndex++;
				}
			}
		}
	}

	m_pContext->PSSetShader(nullptr, nullptr, 0);
}

// 頂点シェーダを設定
void Renderings::Model3DRenderer::SetVertexShader(const VertexShader* pVertexShader)
{
	m_pVertexShader = pVertexShader;
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

	// カメラに対応した描画オブジェクトリストが見つからなければ生成
	auto cameraIt = m_renderObjects.find(pICameraScreen);
	if (cameraIt == m_renderObjects.end())
	{
		m_renderObjects.emplace(pICameraScreen, std::unordered_map<const Model3DSource*, RenderObjects>{});
		cameraIt = m_renderObjects.find(pICameraScreen);
	}

	// モデルソースに対応した描画オブジェクトリストが見つからなければ生成
	auto modelIt = cameraIt->second.find(pModelSource);
	if (modelIt == cameraIt->second.end())
	{
		cameraIt->second.emplace(pModelSource, RenderObjects{});
		modelIt = cameraIt->second.find(pModelSource);
	}

	// モデルのポインタの追加
	modelIt->second.pModels.push_back(pModel);
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
	auto& pModels = m_renderObjects.at(pICameraScreen).at(pModelSource).pModels;

	auto it = std::ranges::find(pModels, pModel);
	if (it != pModels.end())
	{
		pModels.erase(it);
	}

	// 空の配列を削除
	if (pModels.size() == 0 && m_renderObjects.at(pICameraScreen).at(pModelSource).pParticleElements.size() == 0)
	{
		m_renderObjects.at(pICameraScreen).erase(pModelSource);
		if (m_renderObjects.at(pICameraScreen).size() == 0)
		{
			m_renderObjects.erase(pICameraScreen);
		}
	}
}

// パーティクルのポインタを追加
void Renderings::Model3DRenderer::AddPParticle(const ICameraScreen* pICameraScreen, const Particle::RenderElement* pParticleElement)
{
	if (!pICameraScreen)
	{
		return;
	}

	// モデルソース
	const Model3DSource* pModelSource = pParticleElement->pModelSource;
	if (!pModelSource)
	{
		return;
	}

	// カメラに対応した描画オブジェクトリストが見つからなければ生成
	auto cameraIt = m_renderObjects.find(pICameraScreen);
	if (cameraIt == m_renderObjects.end())
	{
		m_renderObjects.emplace(pICameraScreen, std::unordered_map<const Model3DSource*, RenderObjects>{});
		cameraIt = m_renderObjects.find(pICameraScreen);
	}

	// モデルソースに対応した描画オブジェクトリストが見つからなければ生成
	auto modelIt = cameraIt->second.find(pModelSource);
	if (modelIt == cameraIt->second.end())
	{
		cameraIt->second.emplace(pModelSource, RenderObjects{});
		modelIt = cameraIt->second.find(pModelSource);
	}

	// モデルのポインタの追加
	modelIt->second.pParticleElements.push_back(pParticleElement);
}

// パーティクルのポインタを削除
void Renderings::Model3DRenderer::RemovePParticle(const ICameraScreen* pICameraScreen, const Particle::RenderElement* pParticleElement)
{
	if (!pICameraScreen)
	{
		return;
	}

	// モデルソース
	const Model3DSource* pModelSource = pParticleElement->pModelSource;
	if (!pModelSource)
	{
		return;
	}

	// ポインタリスト
	auto& pParticleElements = m_renderObjects.at(pICameraScreen).at(pModelSource).pParticleElements;

	auto it = std::ranges::find(pParticleElements, pParticleElement);
	if (it != pParticleElements.end())
	{
		pParticleElements.erase(it);
	}

	// 空の配列を削除
	if (pParticleElements.size() == 0 && m_renderObjects.at(pICameraScreen).at(pModelSource).pModels.size() == 0)
	{
		m_renderObjects.at(pICameraScreen).erase(pModelSource);
		if (m_renderObjects.at(pICameraScreen).size() == 0)
		{
			m_renderObjects.erase(pICameraScreen);
		}
	}
}

// 白画像を作成
void Renderings::Model3DRenderer::CreateWhiteTexture()
{
	// 白色
	const uint32_t white = 0xFFFFFFFF;

	// テクスチャ詳細
	D3D11_TEXTURE2D_DESC desc{};
	desc.Width = 1;
	desc.Height = 1;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	// サブリソースデータ
	D3D11_SUBRESOURCE_DATA data{};
	data.pSysMem = &white;
	data.SysMemPitch = sizeof(uint32_t);

	// テクスチャ
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
	HRESULT hr = m_pDevice->CreateTexture2D
	(
		&desc,
		&data,
		texture.GetAddressOf()
	);
	if (FAILED(hr))
	{
		return;
	}

	// シェーダリソースビュー詳細
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	// シェーダリソースビュー
	hr = m_pDevice->CreateShaderResourceView(
		texture.Get(),
		&srvDesc,
		m_whiteTexture.GetAddressOf()
	);
	if (FAILED(hr))
	{
		return;
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
		m_pVertexShader->GetBlob()->GetBufferPointer(),
		m_pVertexShader->GetBlob()->GetBufferSize(),
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

// パーツの情報を設定
void Renderings::Model3DRenderer::SetPartInfo(const Model3DSource::PartInfo& partInfo)
{
	// テクスチャを設定
	m_pContext->PSSetShaderResources(0, 1, partInfo.texture.Get() ? partInfo.texture.GetAddressOf() : m_whiteTexture.GetAddressOf());

	// 定数バッファ
	ConstantBuffer* pConstantBuffer = m_pPixelShader->GetConstantBuffer();
	// 定数バッファを設定
	pConstantBuffer->SetVariable("DiffuseColor", partInfo.effectInfo.diffuseColor);
	pConstantBuffer->SetVariable("EmissiveColor", partInfo.effectInfo.emissiveColor);
	pConstantBuffer->SetVariable("SpecularColor", partInfo.effectInfo.specularColor);
	pConstantBuffer->SetVariable("SpecularPower", partInfo.effectInfo.specularPower);
	m_pPixelShader->SetConstantBuffer(m_pDevice, m_pContext);
}

// 描画
void Renderings::Model3DRenderer::Draw(const ICameraScreen* pICameraScreen, const DirectX::ModelMeshPart* pPart)
{
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
	m_pContext->DrawIndexed
	(
		pPart->indexCount,
		pPart->startIndex,
		pPart->vertexOffset
	);
}

// インスタンス描画
void Renderings::Model3DRenderer::DrawInstanced(const ICameraScreen* pICameraScreen, const DirectX::ModelMeshPart* pPart, size_t instMatricesCount)
{
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

	// 入力レイアウト
	m_pContext->IASetInputLayout(m_inputLayout.Get());

	// 描画
	m_pContext->DrawIndexedInstanced
	(
		pPart->indexCount,
		static_cast<UINT>(instMatricesCount),
		pPart->startIndex,
		pPart->vertexOffset,
		0
	);
}
