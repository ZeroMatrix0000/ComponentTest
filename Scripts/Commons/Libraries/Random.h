/*
 * FileName:     Random.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/09
 *
 * 乱数
 */

#pragma once

#include <random>

namespace Libraries
{
	// 乱数
	class Random
	{

	public:


		/* 静的関数 */

		// float 型の範囲乱数を取得
		static float GetFloatRange(float min, float max);


	private:


		/* 静的変数 */

		static std::random_device m_rd;
		// 乱数生成器
		static std::mt19937_64 m_mt;

	};
}
