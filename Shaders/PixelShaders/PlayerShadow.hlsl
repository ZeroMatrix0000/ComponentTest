/*
 * FileName:     PlayerShadow.hlsl
 * Author:       Takao Hayata
 * Last Updated: 2026/10/16
 *
 * プレイヤーの丸影
 */

#include "../Header.hlsli"

// テクスチャ
Texture2D<float4> Texture : register(t0);
// サンプラー
SamplerState Sampler : register(s0);

// 環境
cbuffer Environment : register(b0)
{
	// 環境光色
	float3 AmbientColor;
	// 光の方向
	float3 LightDirection;
	// ライトの色
	float3 LightColor;
	// 目線の位置
	float3 EyePosition;
}
// マテリアル
cbuffer Material : register(b1)
{
	// 拡散反射色
	float3 DiffuseColor;
	// 放射色
	float3 EmissiveColor;
	// 鏡面反射色
	float3 SpecularColor;
	// 鏡面反射の強さ
	float SpecularPower;
}
// プレイヤー
cbuffer Shadow : register(b2)
{
	// 影の個数
	int ShadowCount;
	// 影の座標
	float3 ShadowPosition[100];
	// 影の半径
	float ShadowRadius[100];
}

float Shadow(float3 worldPosition, float3 normal)
{
	// 影の濃さ
	float shadow = 0.0f;
	
	for (int i = 0; i < ShadowCount; i++)
	{
		// プレイヤーとの距離
		float3 delta = worldPosition - ShadowPosition[i];
		// 縦に伸ばす
		delta *= float3(1.0f, 0.2f, 1.0f);
		
		shadow = max(shadow, step(length(delta), ShadowRadius[i]) * step(0.1, dot(normal, float3(0.0f, 1.0f, 0.0f))) * 0.4f);
	}

	return shadow;
}

float4 main(PSInput input) : SV_Target
{
	// テクスチャ色
	float4 textureColor = Texture.Sample(Sampler, input.texCoord);
	
	// 環境光色
	float3 ambientColor = textureColor.rgb * AmbientColor * DiffuseColor.rgb * input.color.rgb;
	
	// 法線
	float3 normal = normalize(input.normal);
	// 光の方向
	float3 lightDirection = normalize(-LightDirection);
	// 拡散反射の強さ
	float diffuse = saturate(dot(normal, lightDirection));
	// 拡散反射色
	float3 diffuseColor = textureColor.rgb * DiffuseColor * LightColor * diffuse * input.color.rgb;
	
	// 目線への方向
	float3 eyeDirection = normalize(EyePosition - input.worldPosition);
	// 反射方向
	float3 halfVector = normalize(lightDirection + eyeDirection);
	
	// 鏡面反射
	float specular = pow(saturate(dot(normal, halfVector)), SpecularPower);
	// 鏡面反射色
	float3 specularColor = SpecularColor * LightColor * specular;
	
	// 描画色
	float3 color = ambientColor + diffuseColor + specularColor + EmissiveColor;
	
	// 影の濃さ
	float shadow = Shadow(input.worldPosition, normal);
	
	return float4(color * (1.0f - shadow), textureColor.a);
}