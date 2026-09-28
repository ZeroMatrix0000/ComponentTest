/*
 * FileName:     Math.h
 * Author:       Takao Hayata
 * Last Updated: 2026/09/28
 *
 * 数学関係
 */

#pragma once

#include <cmath>

namespace Libraries
{
	namespace Math
	{
		typedef DirectX::SimpleMath::Color      Color;
		typedef DirectX::SimpleMath::Vector2    Vector2;
		typedef DirectX::SimpleMath::Vector3    Vector3;
		typedef DirectX::SimpleMath::Quaternion Quaternion;
		typedef DirectX::SimpleMath::Matrix     Matrix;
		typedef DirectX::SimpleMath::Plane      Plane;
		typedef DirectX::SimpleMath::Viewport   Viewport;

		// 円周率
		constexpr float PI = DirectX::XM_PI;
		// 極小値
		constexpr float EPSILON = 1e-6f;
		// 極大値
		constexpr float INFTY = 1e30f;

		// NaNかどうか
		inline bool IsNaN(float x) { return std::isnan(x); }

		// 絶対値を求める
		template<typename T>
		constexpr T Abs(T x) { return x < 0 ? x * -1 : x; }

		// 最小値を求める
		template<typename T>
		constexpr T Min(T x, T y) { return x < y ? x : y; }
		// 最小値を求める
		template<typename T, typename... Args> requires
			(std::same_as<Args, T> && ...) &&
			(sizeof...(Args) > 1)
		constexpr T Min(T x, Args... args) { return Min(x, Min(args...)); }

		// 最大値を求める
		template<typename T>
		constexpr T Max(T x, T y) { return x > y ? x : y; }
		// 最大値を求める
		template<typename T, typename... Args> requires
			(std::same_as<Args, T> && ...) &&
			(sizeof...(Args) > 1)
		constexpr T Max(T x, Args... args) { return Max(x, Max(args...)); }

		// 切り捨て
		inline float Floor(float x)    { return std::floorf(x); }
		// 切り捨て
		inline int   FloorInt(float x) { return static_cast<int>(std::floorf(x)); }

		// 四捨五入
		inline float Round(float x)    { return std::roundf(x); }
		// 四捨五入
		inline int   RoundInt(float x) { return static_cast<int>(std::roundf(x)); }

		// 切り上げ
		inline float Ceil(float x)    { return std::ceilf(x); }
		// 切り上げ
		inline int   CeilInt(float x) { return static_cast<int>(std::ceilf(x)); }

		// 範囲内に収める
		template<typename T>
		constexpr T Clamp(T x, T min, T max)
		{
			if (min > max) return Clamp(x, max, min);
			if (x < min)   return min;
			if (x > max)   return max;
			return x;
		}

		// 範囲内に周期的に収める
		constexpr float Cycle(float x, float min, float max)
		{
			if (min > max)  return Cycle(x, max, min);
			if (min == max) return max;
			if (x < min)    return x + Math::Ceil((min - x) / (max - min)) * (max - min);
			if (x > max)    return x - Math::Ceil((x - max) / (max - min)) * (max - min);
			return x;
		}

		// 範囲内に周期的に収める
		constexpr int Cycle(int x, int min, int max)
		{
			if (min > max)  return Cycle(x, max, min);
			if (min == max) return max;
			if (x < min)    return x + (max - x - 1) / (max - min) * (max - min);
			if (x > max)    return x - (x - min - 1) / (max - min) * (max - min);
			return x;
		}

		// 正弦
		inline float Sin(float x) { return std::sinf(x); }
		// 余弦
		inline float Cos(float x) { return std::cosf(x); }
		// 正接
		inline float Tan(float x) { return std::tanf(x); }

		// 逆正弦
		inline float Arcsin(float x) { return std::asinf(x); }
		// 逆余弦
		inline float Arccos(float x) { return std::acosf(x); }
		// 逆正接2
		inline float Arctan2(float y, float x) { return std::atan2f(y, x); }

		// 指数関数
		inline float Exp(float x) { return std::expf(x); }

		// べき関数
		inline float Pow(float x, float y) { return std::powf(x, y); }

		// 平方根
		inline float Sqrt(float x) { return std::sqrtf(x); }

		// 度数法から弧度法に変換
		constexpr float Deg2Rad(float deg) { return deg / 180.f * PI; }
		// 弧度法から度数法に変換
		constexpr float Rad2Deg(float rad) { return rad * 180.f / PI; }

		// 符号を取得
		constexpr int Sign(float x) { return x > 0.0f ? 1 : x < 0.0f ? -1 : 0; }

		// 線形補間
		constexpr float Lerp(float x, float y, float t) { return x * (1.0f - t) + y * t; }
	}
}
