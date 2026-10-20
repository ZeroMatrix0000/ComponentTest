Texture2D Texture : register(t0);
SamplerState Sampler : register(s0);

cbuffer ConstBuffer : register(b0)
{
	float2 TextureSize;
	float Time;
}

struct PSInput
{
	float4 Color : COLOR0;
	float2 TexCoord : TEXCOORD0;
};

float Mod(float x, float y)
{
	float z = x % y;
	return lerp(z, z + y, step(x, 0.0));
}

float4 main(PSInput input) : SV_Target
{
	static const float PI = 3.14159265359f;
	
	static const int N = 3;
	static const float ANGLE = (0.5f - 1.0f / N) * PI;
	static const float SIN_ANGLE = sin(ANGLE);
	
	float2 uv = input.TexCoord;
	float4 color = Texture.Sample(Sampler, uv);
	
	float2 pos = uv * TextureSize + float2(2.0f, -1.0f) * Time * 50.0f;
	float2 modPos = float2(Mod(pos.x, 200.0f) - 100.0f, Mod(pos.y, 200.0f) - 100.0f);
	
	float theta = Mod(atan2(modPos.y, modPos.x) + Time, 2 * PI / N);
	
	float x = SIN_ANGLE / sin(ANGLE + theta);
	
	float s = step(length(modPos), x * 75.0f);
	
	color.rgb *= lerp(float3(1.0f, 1.0f, 1.0f), float3(1.0f, 0.0f, 0.5f), s * 0.2f);
	
	return color * input.Color;
}
