/*
 * FileName:     ColliderRenderer.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/24
 *
 * 当たり判定描画
 */

#include "Pch.h"
#include "ColliderRenderer.h"

#include "../Colliders/BoxCollider.h"
#include "../Colliders/SphereCollider.h"
#include "../Colliders/MeshCollider.h"
#include "../Renderings/ICameraScreen.h"

 // コンストラクタ
Renderings::ColliderRenderer::ColliderRenderer()
	: IColliderRenderer{}
	, m_primitiveBatch{}
	, m_basicEffect{}
	, m_pBoxColliders{}
	, m_pSphereColliders{}
	, m_pMeshColliders{}
	, m_pContext{}
	, m_pCommonStates{}
	, m_inputLayout{}
{
}

// 初期化処理
void Renderings::ColliderRenderer::Initialize(ID3D11Device5* pDevice, ID3D11DeviceContext4* pContext, const DirectX::CommonStates& commonStates)
{
	m_pContext      = pContext;
	m_pCommonStates = &commonStates;

	m_primitiveBatch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>(pContext);
	m_basicEffect    = std::make_unique<DirectX::BasicEffect>(pDevice);
	m_basicEffect->SetVertexColorEnabled(true);
	Utility::ThrowIfFailed
	(
		DirectX::CreateInputLayoutFromEffect<DirectX::VertexPositionColor>(pDevice, m_basicEffect.get(), m_inputLayout.ReleaseAndGetAddressOf())
	);
}

// 描画処理
void Renderings::ColliderRenderer::Render()
{
	m_pContext->OMSetBlendState(m_pCommonStates->Opaque(), nullptr, 0xFFFFFFFF);
	m_pContext->OMSetDepthStencilState(m_pCommonStates->DepthDefault(), 0);
	m_pContext->RSSetState(m_pCommonStates->CullNone());

	m_basicEffect->Apply(m_pContext);
	m_pContext->IASetInputLayout(m_inputLayout.Get());

	// 描画開始
	m_primitiveBatch->Begin();

	// 長方形
	for (const auto* pBoxCollider : m_pBoxColliders)
	{
		// カメラ画面
		for (Ref iCameraScreenRef : pBoxCollider->GetICameraScreenRefs())
		{
			m_basicEffect->SetMatrices(Math::Matrix::Identity, iCameraScreenRef->GetViewMatrix(), iCameraScreenRef->GetProjectionMatrix());
			m_basicEffect->Apply(m_pContext);
			DrawBox(pBoxCollider->GetWorldBox(), pBoxCollider->GetColor());
		}
	}
	// 球
	for (const auto* pSphereCollider : m_pSphereColliders)
	{
		// カメラ画面
		for (Ref iCameraScreenRef : pSphereCollider->GetICameraScreenRefs())
		{
			m_basicEffect->SetMatrices(Math::Matrix::Identity, iCameraScreenRef->GetViewMatrix(), iCameraScreenRef->GetProjectionMatrix());
			m_basicEffect->Apply(m_pContext);
			DrawSphere(pSphereCollider->GetWorldSphere(), pSphereCollider->GetColor(), iCameraScreenRef->GetEyePosition(), 48);
		}
	}
	// メッシュ
	for (const auto* pMeshCollider : m_pMeshColliders)
	{
		// カメラ画面
		for (Ref iCameraScreenRef : pMeshCollider->GetICameraScreenRefs())
		{
			m_basicEffect->SetMatrices(Math::Matrix::Identity, iCameraScreenRef->GetViewMatrix(), iCameraScreenRef->GetProjectionMatrix());
			m_basicEffect->Apply(m_pContext);
			DrawMesh(pMeshCollider->GetWorldMesh(), pMeshCollider->GetColor());
		}
	}

	// 描画終了
	m_primitiveBatch->End();
}

// 長方形の当たり判定のポインタを追加
void Renderings::ColliderRenderer::AddPBoxCollider(const Colliders::BoxCollider* pBoxCollider)
{
	m_pBoxColliders.push_back(pBoxCollider);
}

// 長方形の当たり判定のポインタを削除
void Renderings::ColliderRenderer::RemovePBoxCollider(const Colliders::BoxCollider* pBoxCollider)
{
	auto it = std::ranges::find(m_pBoxColliders, pBoxCollider);
	if (it != m_pBoxColliders.end())
	{
		m_pBoxColliders.erase(it);
	}
}

// 球の当たり判定のポインタを追加
void Renderings::ColliderRenderer::AddPSphereCollider(const Colliders::SphereCollider* pSphereCollider)
{
	m_pSphereColliders.push_back(pSphereCollider);
}

// 球の当たり判定のポインタを削除
void Renderings::ColliderRenderer::RemovePSphereCollider(const Colliders::SphereCollider* pSphereCollider)
{
	auto it = std::ranges::find(m_pSphereColliders, pSphereCollider);
	if (it != m_pSphereColliders.end())
	{
		m_pSphereColliders.erase(it);
	}
}

// メッシュの当たり判定のポインタを追加
void Renderings::ColliderRenderer::AddPMeshCollider(const Colliders::MeshCollider* pMeshCollider)
{
	m_pMeshColliders.push_back(pMeshCollider);
}

// メッシュの当たり判定のポインタを削除
void Renderings::ColliderRenderer::RemovePMeshCollider(const Colliders::MeshCollider* pMeshCollider)
{
	auto it = std::ranges::find(m_pMeshColliders, pMeshCollider);
	if (it != m_pMeshColliders.end())
	{
		m_pMeshColliders.erase(it);
	}
}

// 直方体を描画
void Renderings::ColliderRenderer::DrawBox(const Math::Box& box, const Math::Color& color) const
{
	// 頂点情報
	DirectX::VertexPositionColor vertexes[] =
	{
		DirectX::VertexPositionColor{ Math::Vector3{ -0.5f, -0.5f, -0.5f }, color },
		DirectX::VertexPositionColor{ Math::Vector3{ -0.5f, -0.5f,  0.5f }, color },
		DirectX::VertexPositionColor{ Math::Vector3{ -0.5f,  0.5f, -0.5f }, color },
		DirectX::VertexPositionColor{ Math::Vector3{ -0.5f,  0.5f,  0.5f }, color },
		DirectX::VertexPositionColor{ Math::Vector3{  0.5f, -0.5f, -0.5f }, color },
		DirectX::VertexPositionColor{ Math::Vector3{  0.5f, -0.5f,  0.5f }, color },
		DirectX::VertexPositionColor{ Math::Vector3{  0.5f,  0.5f, -0.5f }, color },
		DirectX::VertexPositionColor{ Math::Vector3{  0.5f,  0.5f,  0.5f }, color }
	};

	uint16_t indexes[] =
	{
		0, 1, 2, 3, 4, 5, 6, 7,
		0, 2, 1, 3, 4, 6, 5, 7,
		0, 4, 1, 5, 2, 6, 3, 7
	};

	// 変換行列
	Math::Matrix matrix =
		Math::Matrix::CreateScale(box.size) *
		Math::Matrix::CreateFromQuaternion(box.rotation) *
		Math::Matrix::CreateTranslation(box.position)
	;
	// 座標を変換
	for(auto& vertex : vertexes)
	{
		vertex.position = Math::Vector3::Transform(vertex.position, matrix);
	}

	m_primitiveBatch->DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY_LINELIST, indexes, 24, vertexes, 8);
}

// 球を描画
void Renderings::ColliderRenderer::DrawSphere(const Math::Sphere& sphere, const Math::Color& color, const Math::Vector3& eyePosition, int segmentCount) const
{
	DirectX::VertexPositionColor* vertexes1 = new DirectX::VertexPositionColor[segmentCount * 2];
	DirectX::VertexPositionColor* vertexes2 = new DirectX::VertexPositionColor[segmentCount * 2];
	DirectX::VertexPositionColor* vertexes3 = new DirectX::VertexPositionColor[segmentCount * 2];
	for (int i = 0; i < segmentCount; i++)
	{
		float angle1 = Math::Deg2Rad(i * 360.0f / segmentCount);
		float angle2 = Math::Deg2Rad((i + 1) * 360.0f / segmentCount);

		vertexes1[i * 2] = DirectX::VertexPositionColor
		{
			sphere.centerPosition + Math::Vector3{ 0.0f, Math::Cos(angle1), Math::Sin(angle1) } * sphere.radius,
			color
		};
		vertexes2[i * 2] = DirectX::VertexPositionColor
		{
			sphere.centerPosition + Math::Vector3{ Math::Sin(angle1), 0.0f, Math::Cos(angle1) } * sphere.radius,
			color
		};
		vertexes3[i * 2] = DirectX::VertexPositionColor
		{
			sphere.centerPosition + Math::Vector3{ Math::Cos(angle1), Math::Sin(angle1), 0.0f } * sphere.radius,
			color
		};
		vertexes1[i * 2 + 1] = DirectX::VertexPositionColor
		{
			sphere.centerPosition + Math::Vector3{ 0.0f, Math::Cos(angle2), Math::Sin(angle2) } * sphere.radius,
			color
		};
		vertexes2[i * 2 + 1] = DirectX::VertexPositionColor
		{
			sphere.centerPosition + Math::Vector3{ Math::Sin(angle2), 0.0f, Math::Cos(angle2) } * sphere.radius,
			color
		};
		vertexes3[i * 2 + 1] = DirectX::VertexPositionColor
		{
			sphere.centerPosition + Math::Vector3{ Math::Cos(angle2), Math::Sin(angle2), 0.0f } * sphere.radius,
			color
		};
	}
	m_primitiveBatch->Draw(D3D11_PRIMITIVE_TOPOLOGY_LINELIST, vertexes1, static_cast<size_t>(segmentCount * 2));
	m_primitiveBatch->Draw(D3D11_PRIMITIVE_TOPOLOGY_LINELIST, vertexes2, static_cast<size_t>(segmentCount * 2));
	m_primitiveBatch->Draw(D3D11_PRIMITIVE_TOPOLOGY_LINELIST, vertexes3, static_cast<size_t>(segmentCount * 2));

	delete[] vertexes1;
	delete[] vertexes2;
	delete[] vertexes3;

	// 球の方向を指すベクトル
	Math::Vector3 targetDirection = sphere.centerPosition - eyePosition;

	// カメラが球の中にあれば何もしない
	if (sphere.radius > targetDirection.Length())
	{
		return;
	}
	// カメラの位置を頂点とし球に接する円錐の頂角を2で割ったもの
	float cornAngle = Math::Arcsin(sphere.radius / targetDirection.Length());

	// 球の方向を指す単位ベクトル
	Math::Vector3 unitZ = targetDirection;
	unitZ.Normalize();

	// カメラと描画する円の距離
	float distance = targetDirection.Length();
	distance *= Math::Cos(cornAngle);
	distance *= Math::Cos(cornAngle);
	// 描画する円の中心
	Math::Vector3 centerPosition = eyePosition + unitZ * distance;
	// 描画する円の半径
	float radius = distance * Math::Tan(cornAngle);

	DrawCircle(Math::Circle{ centerPosition, radius, unitZ }, color, segmentCount);
}

// 円を描画
void Renderings::ColliderRenderer::DrawCircle(const Math::Circle& circle, const Math::Color& color, int segmentCount) const
{
	// 頂点情報
	DirectX::VertexPositionColor* vertexes = new DirectX::VertexPositionColor[segmentCount * 2];

	// 法線ベクトルX
	Math::Vector3 unitX = circle.axis.Cross(Math::Vector3::UnitY);
	// サイズが小さければ別のベクトルをもとに法線を求める
	if (unitX.Length() < Math::EPSILON)
	{
		unitX = circle.axis.Cross(Math::Vector3::UnitZ);
	}
	unitX.Normalize();
	// 法線ベクトルY
	Math::Vector3 unitY = circle.axis.Cross(unitX);

	for (int i = 0; i < segmentCount; i++)
	{
		float angle1 = Math::Deg2Rad(i * 360.0f / segmentCount);
		float angle2 = Math::Deg2Rad((i + 1) * 360.0f / segmentCount);

		vertexes[i * 2] = DirectX::VertexPositionColor
		{
			circle.centerPosition + (unitX * Math::Cos(angle1) + unitY * Math::Sin(angle1)) * circle.radius,
			color
		};
		vertexes[i * 2 + 1] = DirectX::VertexPositionColor
		{
			circle.centerPosition + (unitX * Math::Cos(angle2) + unitY * Math::Sin(angle2)) * circle.radius,
			color
		};
	}
	m_primitiveBatch->Draw(D3D11_PRIMITIVE_TOPOLOGY_LINELIST, vertexes, static_cast<size_t>(segmentCount * 2));

	delete[] vertexes;
}

// 三角形を描画
void Renderings::ColliderRenderer::DrawTriangle(const Math::Triangle& triangle, const Math::Color& color) const
{
	m_primitiveBatch->DrawLine
	(
		DirectX::VertexPositionColor{ triangle.v1, color },
		DirectX::VertexPositionColor{ triangle.v2, color }
	);
	m_primitiveBatch->DrawLine
	(
		DirectX::VertexPositionColor{ triangle.v2, color },
		DirectX::VertexPositionColor{ triangle.v3, color }
	);
	m_primitiveBatch->DrawLine
	(
		DirectX::VertexPositionColor{ triangle.v3, color },
		DirectX::VertexPositionColor{ triangle.v1, color }
	);

	// 重心
	Math::Vector3 center = triangle.GetCenter();
	// 法線
	Math::Vector3 normal = triangle.GetNormal();

	m_primitiveBatch->DrawLine
	(
		DirectX::VertexPositionColor{ center, color },
		DirectX::VertexPositionColor{ center + normal, color }
	);
}

// メッシュを描画
void Renderings::ColliderRenderer::DrawMesh(const Mesh& mesh, const Math::Color& color) const
{
	// 各三角形を描画
	for (const auto& face : mesh.f)
	{
		DrawTriangle(Math::Triangle{ mesh.v.at(face.at(0)), mesh.v.at(face.at(1)), mesh.v.at(face.at(2)) }, color);
	}
}
