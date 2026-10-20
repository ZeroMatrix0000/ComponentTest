/*
 * FileName:     InstancingTest.hlsl
 * Author:       Takao Hayata
 * Last Updated: 2026/10/20
 *
 * インスタンシング描画テスト頂点シェーダ
 */

#include "../Header.hlsli"

// 定数バッファ
cbuffer Camera : register(b0)
{
	// ビュー行列
	float4x4 View;
	// プロジェクション行列
	float4x4 Projection;
}

VSOutput main(VSInput input)
{
	VSOutput output;
	
	// ワールド座標
	float3 worldPosition = mul(float4(input.position, 1.0f), input.world);
	
	// 行列を適用
	output.position = float4(worldPosition, 1.0f);
	output.position = mul(output.position, View);
	output.position = mul(output.position, Projection);
	
	output.texCoord = input.texCoord;
	output.worldPosition = worldPosition;
	output.normal = mul(input.normal, (float3x3) input.world);
	
	output.color = input.color;
	
	return output;
}