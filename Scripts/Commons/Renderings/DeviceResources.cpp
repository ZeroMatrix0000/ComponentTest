/*
 * FileName:     DeviceResources.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/07/03
 *
 * Direct3D 11に必要なリソース
 */

#include "Pch.h"
#include "DeviceResources.h"

 // コンストラクタ
Renderings::DeviceResources::DeviceResources()
	: OnlyOne{ typeid(DeviceResources) }
	, m_hWindow{}
	, m_outputSize{}
	, m_featureLevelSupported{}
	, m_d3dDevice{}
	, m_d3dContext{}
	, m_swapChain{}
	, m_renderTargetView{}
	, m_viewPort{}
	, m_depthStencil{}
	, m_depthStencilView{}
{
}

// 初期化処理
void Renderings::DeviceResources::Initialize(const HWND& hWindow)
{
	m_hWindow = hWindow;

	// クライアントサイズ
	RECT clientRect{};
	GetClientRect(hWindow, &clientRect);

	// 出力サイズを設定
	m_outputSize.x = clientRect.right - clientRect.left;
	m_outputSize.y = clientRect.bottom - clientRect.top;

	// デバイスとコンテキストの作成
	CreateDevice();
	// スワップチェインの作成
	CreateSwapChain();
	// 描画ターゲットビューの作成
	CreateRenderTargetView();
	// 深度ステンシルビューの作成
	CreateDepthStencilView();
}

// 画面初期化
void Renderings::DeviceResources::Clear()
{
	// 背景色
	static constexpr Math::Color BACK_GROUND_COLOR = Math::Color{ 0.2f, 0.5f, 0.8f, 1.0f };

	// 描画ターゲットビューのクリア
	m_d3dContext->ClearRenderTargetView
	(
		m_renderTargetView.Get(),
		BACK_GROUND_COLOR
	);

	// 深度ステンシルビューのクリア
	m_d3dContext->ClearDepthStencilView
	(
		m_depthStencilView.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0
	);

	// 描画ターゲットを設定
	m_d3dContext->OMSetRenderTargets
	(
		1,
		m_renderTargetView.GetAddressOf(),
		m_depthStencilView.Get()
	);

	// ビューポートを設定
	m_d3dContext->RSSetViewports(1, &m_viewPort);
}

// 画面表示
void Renderings::DeviceResources::Present(bool isLimitFPS)
{
	// バックバッファを表示
	if (isLimitFPS)
	{
		Utility::ThrowIfFailed(m_swapChain->Present(1, 0));
	}
	else
	{
		Utility::ThrowIfFailed(m_swapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING));
	}
}

// ウィンドウサイズ変更時の処理
void Renderings::DeviceResources::OnWindowSizeChanged(const Math::Vector2Int& outputSize)
{
	// ウィンドウの大きさが変わらなければ何もしない
	if (outputSize.x == m_outputSize.x && outputSize.y == m_outputSize.y)
	{
		return;
	}

	// ウィンドウの大きさを変更
	m_outputSize = outputSize;

	// 描画ターゲットの解除
	m_d3dContext->OMSetRenderTargets(0, nullptr, nullptr);
	// 描画ターゲットビューの開放
	m_renderTargetView.Reset();
	// 深度ステンシルテクスチャの開放
	m_depthStencil.Reset();
	// 深度ステンシルビューの開放
	m_depthStencilView.Reset();

	// バッファサイズを変更
	Utility::ThrowIfFailed(m_swapChain->ResizeBuffers
	(
		2,
		outputSize.x,
		outputSize.y,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING
	));

	// 描画ターゲットビューの作成
	CreateRenderTargetView();
	// 深度ステンシルビューの作成
	CreateDepthStencilView();
}

// DXGIデバイスを作成
Microsoft::WRL::ComPtr<IDXGIDevice4> Renderings::DeviceResources::CreateDXGIDevice() const
{
	Microsoft::WRL::ComPtr<IDXGIDevice4> dxgiDevice{};
	m_d3dDevice.As(&dxgiDevice);
	return dxgiDevice;
}

// デバイスとコンテキストの作成
void Renderings::DeviceResources::CreateDevice()
{
	// 機能レベルの配列
	static constexpr D3D_FEATURE_LEVEL FEATURE_LEVELS[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
	};
	// 機能レベル配列の要素数
	static constexpr UINT              FEATURE_LEVELS_COUNT = 7;

	// デバイス作成オプション
	// Direct2Dと相互運用
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
// デバッグビルドなら
#if defined(_DEBUG)
	// デバッグレイヤーを追加
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// デバイス
	Microsoft::WRL::ComPtr<ID3D11Device>        device;
	// コンテキスト
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;

	// デバイスとコンテキストの作成
	Utility::ThrowIfFailed(D3D11CreateDevice
	(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		creationFlags,
		FEATURE_LEVELS,
		FEATURE_LEVELS_COUNT,
		D3D11_SDK_VERSION,
		device.GetAddressOf(),
		&m_featureLevelSupported,
		context.GetAddressOf()
	));
	Utility::ThrowIfFailed(device.As(&m_d3dDevice));
	Utility::ThrowIfFailed(context.As(&m_d3dContext));
}

// スワップチェインの作成
void Renderings::DeviceResources::CreateSwapChain()
{
	// ファクトリー作成フラグ
	UINT flags = 0;
// デバッグビルドなら
#if defined(_DEBUG)
	flags = DXGI_CREATE_FACTORY_DEBUG;
#endif

	// DXGIファクトリー
	Microsoft::WRL::ComPtr<IDXGIFactory5> dxgiFactory;
	Utility::ThrowIfFailed(CreateDXGIFactory2(flags, IID_PPV_ARGS(dxgiFactory.GetAddressOf())));

	// スワップチェインの設定
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width              = m_outputSize.x;
	swapChainDesc.Height             = m_outputSize.y;
	swapChainDesc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
	//swapChainDesc.Stereo             = false;
	swapChainDesc.SampleDesc.Count   = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount        = 2;
	swapChainDesc.Scaling            = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect         = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode          = DXGI_ALPHA_MODE_IGNORE;
	swapChainDesc.Flags              = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

	// スワップチェインの全画面表示モードの設定
	DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFSDesc{};
	//swapChainFSDesc.RefreshRate.Numerator   = 60;
	//swapChainFSDesc.RefreshRate.Denominator = 1;
	//swapChainFSDesc.ScanlineOrdering        = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	//swapChainFSDesc.Scaling                 = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainFSDesc.Windowed                = true;

	// スワップチェイン
	Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain;

	// スワップチェインの作成
	Utility::ThrowIfFailed(dxgiFactory->CreateSwapChainForHwnd
	(
		m_d3dDevice.Get(),
		m_hWindow,
		&swapChainDesc,
		&swapChainFSDesc,
		nullptr,
		swapChain.GetAddressOf()
	));
	Utility::ThrowIfFailed(swapChain.As(&m_swapChain));

	// 親ファクトリの取得
	Microsoft::WRL::ComPtr<IDXGIFactory> factory;
	m_swapChain->GetParent(IID_PPV_ARGS(factory.GetAddressOf()));
	// Alt+Enterの無効化
	factory->MakeWindowAssociation(m_hWindow, DXGI_MWA_NO_ALT_ENTER);
}

// 描画ターゲットビューの作成
void Renderings::DeviceResources::CreateRenderTargetView()
{
	// バックバッファ
	Microsoft::WRL::ComPtr<ID3D11Texture2D1> renderTarget;
	Utility::ThrowIfFailed(m_swapChain->GetBuffer(0, IID_PPV_ARGS(renderTarget.GetAddressOf())));

	// 描画ターゲットビューの作成
	Utility::ThrowIfFailed(m_d3dDevice->CreateRenderTargetView
	(
		renderTarget.Get(),
		nullptr,
		m_renderTargetView.GetAddressOf()
	));
}

// 深度ステンシルビューの作成
void Renderings::DeviceResources::CreateDepthStencilView()
{
	// ビューポートの設定
	m_viewPort.TopLeftX = 0.0f;
	m_viewPort.TopLeftY = 0.0f;
	m_viewPort.Width    = static_cast<float>(m_outputSize.x);
	m_viewPort.Height   = static_cast<float>(m_outputSize.y);
	m_viewPort.MinDepth = 0.0f;
	m_viewPort.MaxDepth = 1.0f;

	// 深度ステンシルテクスチャの設定
	D3D11_TEXTURE2D_DESC1 descDepth{};
	descDepth.Width              = m_outputSize.x;
	descDepth.Height             = m_outputSize.y;
	descDepth.MipLevels          = 1;
	descDepth.ArraySize          = 1;
	descDepth.Format             = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count   = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage              = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags          = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags     = 0;
	descDepth.MiscFlags          = 0;
	//descDepth.TextureLayout      = D3D11_TEXTURE_LAYOUT_UNDEFINED;

	// 深度ステンシルテクスチャの作成
	Utility::ThrowIfFailed(m_d3dDevice->CreateTexture2D1
	(
		&descDepth,
		nullptr,
		m_depthStencil.GetAddressOf()
	));

	// 深度ステンシルビューの作成
	Utility::ThrowIfFailed(m_d3dDevice->CreateDepthStencilView
	(
		m_depthStencil.Get(),
		nullptr,
		m_depthStencilView.GetAddressOf()
	));
}
