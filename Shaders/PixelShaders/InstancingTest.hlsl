/*
 * FileName:     InstancingTest.hlsl
 * Author:       Takao Hayata
 * Last Updated: 2026/10/08
 *
 * インスタンシング描画テストピクセルシェーダ
 */

// テクスチャ
Texture2D<float4> Texture : register(t0);
// サンプラー
SamplerState Sampler : register(s0);

// 定数バッファ
cbuffer ConstantBuffer : register(b0)
{
	// 拡散反射色
	float3 DiffuseColor;
	// 放射色
	float3 EmissiveColor;
	// 鏡面反射色
	float3 SpecularColor;
	// 鏡面反射の強さ
	float SpecularPower;
	
	// 環境光色
	float3 AmbientColor;
	// 光の方向
	float3 LightDirection;
	// ライトの色
	float3 LightColor;
	// 目線の位置
	float3 EyePosition;
}

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
	// テクスチャ色
	float4 textureColor = Texture.Sample(Sampler, input.texCoord);
	
	// 環境光色
	float3 ambientColor = textureColor.rgb * AmbientColor * DiffuseColor.rgb;
	
	// 法線
	float3 normal = normalize(input.normal);
	// 光の方向
	float3 lightDirection = normalize(-LightDirection);
	// 拡散反射の強さ
	float diffuse = saturate(dot(normal, lightDirection));
	// 拡散反射色
	float3 diffuseColor = textureColor.rgb * DiffuseColor * LightColor * diffuse;
	
	// 目線への方向
	float3 eyeDirection = normalize(EyePosition - input.worldPosition);
	// 反射方向
	float3 halfVector = normalize(lightDirection + eyeDirection);
	
	float specular = pow(saturate(dot(normal, halfVector)), SpecularPower);
	// 鏡面反射色
	float3 specularColor = SpecularColor * LightColor * specular;
	
	// 描画色
	float3 color = ambientColor + diffuseColor + specularColor + EmissiveColor;
	
	return float4(color, textureColor.a);
}