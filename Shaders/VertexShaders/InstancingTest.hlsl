/*
 * FileName:     InstancingTest.hlsl
 * Author:       Takao Hayata
 * Last Updated: 2026/10/08
 *
 * インスタンシング描画テスト頂点シェーダ
 */

// 定数バッファ
cbuffer ConstantBuffer : register(b0)
{
	// ビュー行列
	float4x4 View;
	// プロジェクション行列
	float4x4 Projection;
}

// 入力
struct VSInput
{
	// 座標
	float3 position : SV_Position;
	// 法線
	float3 normal : NORMAL;
	// UV座標
	float2 texCoord : TEXCOORD0;
	
	// ワールド行列
	float4x3 world : InstMatrix;
};

// 出力
struct VSOutput
{
	// 座標
	float4 position : SV_Position;
	// UV座標
	float2 texCoord : TEXCOORD0;
	
	// ワールド座標
	float3 worldPosition : TEXCOORD1;
	// 法線
	float3 normal : TEXCOORD2;
};

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
	output.normal = mul(input.normal, float3x3(input.world._11_12_13, input.world._21_22_23, input.world._31_32_33));
	
	return output;
}