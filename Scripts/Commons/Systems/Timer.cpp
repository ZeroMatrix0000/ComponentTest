/*
 * FileName:     Timer.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/09
 *
 * タイマー
 */

#include "Pch.h"
#include "Timer.h"

// コンストラクタ
Systems::Timer::Timer()
	: ITimer{}
	, m_time{}
	, m_oldTime{}
	, m_fpsCount{}
	, m_fps{}
	, m_fpsTimer{}
{
}

// 初期化処理
void Systems::Timer::Initialize()
{
	// 経過時間の初期化
	QueryPerformanceCounter(&m_oldTime);
	m_time = m_oldTime;
}

// 更新処理
void Systems::Timer::Update()
{
	// 経過時間の取得
	m_oldTime = m_time;
	QueryPerformanceCounter(&m_time);

	m_fpsCount++;
	m_fpsTimer += GetDeltaTime();
	if (m_fpsTimer >= 1.0f)
	{
		m_fps = m_fpsCount;
		m_fpsCount = 0;
		m_fpsTimer--;
	}
}
