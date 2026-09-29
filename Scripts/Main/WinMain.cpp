/*
 * FileName:     WinMain.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/07/07
 *
 * メイン
 */

#include "Pch.h"

#include "Game.h"

/* 前方宣言 */

// ウィンドウプロシージャ
LRESULT CALLBACK WndProcW(HWND, UINT, WPARAM, LPARAM);

/* 定数 */

// ウィンドウクラス名
static constexpr LPCWSTR WINDOW_CLASS_NAME = L"ComponentTestWindowClass";
// ウィンドウ名
static constexpr LPCWSTR WINDOW_NAME       = L"ComponentTest";
// 出力幅
static constexpr int     OUTPUT_WIDTH      = 1600;
// 出力高さ
static constexpr int     OUTPUT_HEIGHT     = 900;

/* 関数 */

// エントリーポイント
int WINAPI wWinMain
(
	_In_     HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_     LPWSTR    lpCmdLine,
	_In_     int       nShowCmd
)
{
	// ゲーム
	Game game{};

	// ウィンドウ
	HWND hWindow{};
	// ウィンドウを作成
	{
		// ウィンドウクラス
		WNDCLASSEXW wc{};
		wc.cbSize        = sizeof(WNDCLASSEXW);
		wc.style         = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc   = WndProcW;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = hInstance;
		wc.hIcon         = LoadIconW(nullptr, IDI_APPLICATION);
		wc.hCursor       = LoadCursorW(nullptr, IDC_ARROW);
		wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
		wc.lpszMenuName  = nullptr;
		wc.lpszClassName = WINDOW_CLASS_NAME;
		wc.hIconSm       = LoadIconW(nullptr, IDI_APPLICATION);
		// ウィンドウクラスを登録
		if (!RegisterClassExW(&wc))
		{
			return 1;
		}

		// ワークエリアサイズ
		RECT workArea{};
		SystemParametersInfoW(SPI_GETWORKAREA, 0, &workArea, 0);

		// ウィンドウサイズ
		RECT wndRect{ 0, 0, OUTPUT_WIDTH, OUTPUT_HEIGHT };
		AdjustWindowRect(&wndRect, WS_OVERLAPPEDWINDOW, false);

		// ウィンドウを作成
		// ワークエリアの中央にウィンドウを配置する
		hWindow = CreateWindowExW
		(
			0,
			WINDOW_CLASS_NAME,
			WINDOW_NAME,
			WS_OVERLAPPEDWINDOW,
			(workArea.right - workArea.left - wndRect.right + wndRect.left) / 2,
			(workArea.bottom - workArea.top - wndRect.bottom + wndRect.top) / 2,
			wndRect.right - wndRect.left,
			wndRect.bottom - wndRect.top,
			nullptr,
			nullptr,
			hInstance,
			&game
		);
		if (!hWindow)
		{
			return 1;
		}

		// ウィンドウを表示
		ShowWindow(hWindow, SW_SHOW);
	}

	// ゲームの初期化
	game.Initialize(hWindow);

	// メッセージ情報
	MSG msg{};
	// 終了要求が出るまで繰り返し
	while (msg.message != WM_QUIT)
	{
		// メッセージを受信したら処理
		while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

		// ゲームの更新
		game.Update();
		// ゲームの描画
		game.Render();
	}

	// ゲームの終了
	game.Finalize();

	return static_cast<int>(msg.wParam);
}

// ウィンドウプロシージャ
LRESULT CALLBACK WndProcW
(
	HWND   hWnd,
	UINT   message,
	WPARAM wParam,
	LPARAM lParam
)
{
	// ゲームへのポインタ
	static Game* s_game{};

	switch (message)
	{
	// ウィンドウを作成
	case WM_CREATE:
		{
			// ゲームを受け取る
			CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
			SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));

			// ゲームへのポインタ
			s_game = reinterpret_cast<Game*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));

			// 1秒毎に処理を実行
			SetTimer(hWnd, 1, 500, nullptr);
		}
		break;
	// ウィンドウを破棄
	case WM_DESTROY:
		PostQuitMessage(0);
		// タイマーを破壊
		KillTimer(hWnd, 1);
		break;
	// クライアント領域上でカーソルが移動したとき
	case WM_MOUSEMOVE:
	// クライアント領域上で左クリックが離されたとき
	case WM_LBUTTONUP:
		DirectX::Mouse::ProcessMessage(message, wParam, lParam);
		break;
	// ウィンドウサイズ変更
	case WM_SIZE:
		// ウィンドウサイズを取得し適用
		s_game->OnWindowSizeChanged(Math::Vector2Int{ LOWORD(lParam), HIWORD(lParam) });
		// ゲームの描画
		s_game->Render();
		break;
	// ウィンドウの背景削除
	case WM_ERASEBKGND:
		return 0;
	// タイマー
	case WM_TIMER:
		{
			// ウィンドウに表示するテキスト
			std::wstring text = WINDOW_NAME;
			text += Utility::FormatWString(L" | FPS: %.2f", 1.0f / s_game->GetDeltaTime());
			// テキストを変更
			SetWindowTextW(hWnd, text.c_str());
		}
		break;
	// ウィンドウのアクティブ状態変更時
	case WM_ACTIVATEAPP:
		DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
		DirectX::Mouse::ProcessMessage(message, wParam, lParam);
		break;
	case WM_ACTIVATE:
	case WM_INPUT:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
		DirectX::Mouse::ProcessMessage(message, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
		DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
		break;
	// 文字入力
	case WM_CHAR:
		{
			// 文字を取得
			wchar_t ch = static_cast<wchar_t>(wParam);
			//s_game->GetTextInput().AddChar(ch);
		}
		break;
	// その他
	default:
		break;
	}

	// 規定のウィンドウプロシージャを使用
	return DefWindowProcW(hWnd, message, wParam, lParam);
}
