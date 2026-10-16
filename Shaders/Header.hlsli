/*
 * FileName:     Header.hlsli
 * Author:       Takao Hayata
 * Last Updated: 2026/10/16
 *
 * シェーダ用ヘッダ
 */

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
	// 色
	float4 color : COLOR;
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
	// 色
	float4 color : COLOR;
};

// 出力
struct PSInput
{
	// 座標
	float4 position : SV_Position;
	// UV座標
	float2 texCoord : TEXCOORD0;
	
	// ワールド座標
	float3 worldPosition : TEXCOORD1;
	// 法線
	float3 normal : TEXCOORD2;
	// 色
	float4 color : COLOR;
};
