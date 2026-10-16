/*
 * FileName:     PlayerSilhouette.hlsl
 * Author:       Takao Hayata
 * Last Updated: 2026/10/16
 *
 * プレイヤーのシルエット
 */

#include "../Header.hlsli"

// テクスチャ
Texture2D<float4> Texture : register(t0);
// サンプラー
SamplerState Sampler : register(s0);

float4 main(PSInput input) : SV_Target
{
	
	return float4(0.0f, 0.0f, 0.0f, 0.4f);
}