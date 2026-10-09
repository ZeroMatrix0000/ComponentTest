/*
 * FileName:     Random.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/09
 *
 * 乱数
 */

#include "Pch.h"

std::random_device Libraries::Random::m_rd{};
std::mt19937_64 Libraries::Random::m_mt{ m_rd() };

// float 型の範囲乱数を取得
float Libraries::Random::GetFloatRange(float min, float max)
{
	std::uniform_real_distribution<float> dist{ min, max };

	return dist(m_mt);
}
