/*
 * FileName:     ResourcesInitializer.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/23
 *
 * リソース初期化
 */

#include "Pch.h"
#include "ResourcesInitializer.h"

#include "../Commons/Renderings/DeviceResources.h"
#include "../Commons/Renderings/RenderingResources.h"
#include "../Commons/Systems/Resources.h"

// リソース初期化
void ResourcesInitializer::InitializeResources
(
	const Renderings::DeviceResources&    deviceResources,
	const Renderings::RenderingResources& renderingResources,
	Ref<Systems::Resources>               resources
)
{
	// デバイス
	auto* device = deviceResources.GetD3DDevice();

	// エフェクトファクトリー
	auto* fx = renderingResources.GetEffectFactory();

	// 画像の読み込み
	resources->LoadImageSources(device, L"Resources\\Images");
	// モデルソースの読み込み
	resources->LoadModelSources(device, fx, L"Resources\\Models");
	// Jsonの読み込み
	resources->LoadJsons(L"Resources\\Jsons");
	resources->AddJsonFromStr
	(
		"Prefab_ErrorMessage",
		R"(
{
	"Text": {
		"Str": "",
		"FontName": "GenEi M Gothic v2",
		"FontSize": 24.0,
		"FontColor": {
			"R": 1.0,
			"G": 1.0,
			"B": 0.0,
			"A": 1.0
		},
		"OutlineWidth": 2.0,
		"OrderInLayer": 65535
	},
	"RectTransform": {
		"Position": {
			"X": 10.0,
			"Y": 10.0
		},
		"Size": {
			"X": 100000.0,
			"Y": 100000.0
		},
		"Pivot": "TopLeft",
		"Anchor": "TopLeft"
	}
}
		)"
	);
	resources->AddJsonFromStr
	(
		"Prefab_Canvas",
		R"(
{
	"Canvas": {
	}
}
		)"
	);
	resources->AddJsonFromStr
	(
		"Prefab_SceneTransitionAnimation",
		R"(
{
	"Image": {
		"ImageSourceName": "Box",
		"Color": {
			"R": 0.0,
			"G": 0.0,
			"B": 0.0,
			"A": 1.0
		},
		"OrderInLayer": 32767
	},
	"RectTransform": {
		"Angle": 15.0
	}
}
		)"
	);
	// メッシュの読み込み
	resources->LoadMeshes(L"Resources\\Meshes");
	// 頂点シェーダの読み込み
	resources->LoadVertexShaders(device, L"Resources\\Shaders\\VertexShaders");
	// ピクセルシェーダの読み込み
	resources->LoadPixelShaders(device, L"Resources\\Shaders\\PixelShaders");

	// シェーダを適用
	resources->GetEditableModelSource("NewGrounds")->SetPixelShader(&resources->GetPixelShader("PlayerShadow").Get());
	resources->GetEditableModelSource("PlayerSilhouette")->SetPixelShader(&resources->GetPixelShader("PlayerSilhouette").Get());

	// 描画順を設定
	resources->GetEditableModelSource("PlayerSilhouette")->SetDrawingOrder(1000);
	resources->GetEditableModelSource("Player")->SetDrawingOrder(2000);
	resources->GetEditableModelSource("NPC")->SetDrawingOrder(2000);
	resources->GetEditableModelSource("Box")->SetDrawingOrder(2000);

	// 深度ステンシルステートの詳細
	D3D11_DEPTH_STENCIL_DESC desc = {};
	// 深度テスト
	desc.DepthEnable = true;
	// 深度を変更しない
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	// 深度が大きければ描画
	desc.DepthFunc = D3D11_COMPARISON_GREATER;
	// ステンシルテスト
	desc.StencilEnable = true;
	desc.StencilReadMask = 0xFF;
	desc.StencilWriteMask = 0xFF;

	// ステンシル操作の詳細
	D3D11_DEPTH_STENCILOP_DESC stencilOp = {};
	// ステンシル値が 0 の場合のみ描画
	stencilOp.StencilFunc = D3D11_COMPARISON_EQUAL;
	// 描画成功時に 値を 増やす
	stencilOp.StencilPassOp = D3D11_STENCIL_OP_INCR;
	// 描画失敗時は何もしない
	stencilOp.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	stencilOp.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

	desc.FrontFace = stencilOp;
	desc.BackFace = stencilOp;

	// 深度ステンシルステートを設定
	resources->GetEditableModelSource("PlayerSilhouette")->SetDepthStencilState(desc, device);
}
