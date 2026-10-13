/*
 * FileName:     PlayerSilhouette.hlsl
 * Author:       Takao Hayata
 * Last Updated: 2026/10/13
 *
 * プレイヤーのシルエット
 */

// テクスチャ
Texture2D<float4> Texture : register(t0);
// サンプラー
SamplerState Sampler : register(s0);

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
};

float4 main(PSInput input) : SV_Target
{
	
	return float4(0.0f, 0.0f, 0.0f, 0.4f);
}