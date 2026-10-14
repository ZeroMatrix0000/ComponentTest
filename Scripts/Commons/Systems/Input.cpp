/*
 * FileName:     Input.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/07/29
 *
 * 入力管理
 */

#include "Pch.h"
#include "Input.h"

 // コンストラクタ
Systems::Input::Input()
	: IInput{}
	, m_hWindow{}
	, m_keyboard{}
	, m_keyboardState{}
	, m_keyboardStateOld{}
	, m_mouse{}
	, m_mouseState{}
	, m_mouseStateOld{}
	, m_mousePositionLoop{}
{
}

// 初期化処理
void Systems::Input::Initialize(const HWND& hWindow)
{
	m_hWindow = hWindow;

	m_keyboardState = m_keyboard.GetState();
	m_mouseState    = m_mouse.GetState();
	MousePositionCorrect();
}

// 更新処理
void Systems::Input::Update()
{
	m_keyboardStateOld = m_keyboardState;
	m_keyboardState    = m_keyboard.GetState();
	m_mouseStateOld    = m_mouseState;
	m_mouseState       = m_mouse.GetState();
	MousePositionCorrect();
}

bool Systems::Input::GetMouseButton(MouseButtonName mouseButtonName) const
{
	switch (mouseButtonName)
	{
	case MouseButtonName::Left:
		return m_mouseState.leftButton;
	case MouseButtonName::Middle:
		return m_mouseState.middleButton;
	case MouseButtonName::Right:
		return m_mouseState.rightButton;
	default:
		return false;
	}
}

// マウスのボタンの離状態を取得
bool Systems::Input::GetMouseButtonUp(MouseButtonName mouseButtonName) const
{
	switch (mouseButtonName)
	{
	case MouseButtonName::Left:
		return !m_mouseState.leftButton   && m_mouseStateOld.leftButton;
	case MouseButtonName::Middle:
		return !m_mouseState.middleButton && m_mouseStateOld.middleButton;
	case MouseButtonName::Right:
		return !m_mouseState.rightButton  && m_mouseStateOld.rightButton;
	default:
		return false;
	}
}

// マウスカーソルを表示
void Systems::Input::DisplayCursor(bool isShow)
{
	ShowCursor(isShow);
}

// マウスの座標の計算
void Systems::Input::MousePositionCorrect()
{
	// マウスの位置
	POINT mousePosition{};
	GetCursorPos(&mousePosition);

	// マウス座標をループさせるなら
	if (m_mousePositionLoop)
	{
		// ウィンドウの長方形
		RECT windowRect{};

		GetWindowRect(m_hWindow, &windowRect);

		// ウィンドウの外にカーソルがある場合ウィンドウ内に収める
		mousePosition.x = Math::Cycle
		(
			static_cast<int>(mousePosition.x),
			static_cast<int>(windowRect.left) + 10,
			static_cast<int>(windowRect.right) - 11
		);
		mousePosition.y = Math::Cycle
		(
			static_cast<int>(mousePosition.y),
			static_cast<int>(windowRect.top) + 31,
			static_cast<int>(windowRect.bottom) - 11
		);

		// 変更前の座標
		POINT oldMousePosition{};
		GetCursorPos(&oldMousePosition);
		// 座標を変更
		SetCursorPos(mousePosition.x, mousePosition.y);
		// 変更した分ずらす
		m_mouseStateOld.x += mousePosition.x - oldMousePosition.x;
		m_mouseStateOld.y += mousePosition.y - oldMousePosition.y;
	}

	m_mouseState.x = mousePosition.x;
	m_mouseState.y = mousePosition.y;
}

// マウスのボタンの押状態を取得
bool Systems::Input::GetMouseButtonDown(MouseButtonName mouseButtonName) const
{
	switch (mouseButtonName)
	{
	case MouseButtonName::Left:
		return m_mouseState.leftButton   && !m_mouseStateOld.leftButton;
	case MouseButtonName::Middle:
		return m_mouseState.middleButton && !m_mouseStateOld.middleButton;
	case MouseButtonName::Right:
		return m_mouseState.rightButton  && !m_mouseStateOld.rightButton;
	default:
		return false;
	}
}
