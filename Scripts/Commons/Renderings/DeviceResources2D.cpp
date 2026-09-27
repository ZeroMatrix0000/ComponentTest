/*
 * FileName:     DeviceResources2D.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/09/27
 *
 * Direct2D に必要なリソース
 */

#include "Pch.h"
#include "DeviceResources2D.h"

#include "DeviceResources.h"

// コンストラクタ
Renderings::DeviceResources2D::DeviceResources2D(const DeviceResources& deviceResources)
	: OnlyOne{ typeid(DeviceResources2D) }
	, m_d2dFactory{}
	, m_d2dDevice{}
	, m_d2dContext{}
	, m_backBuffer{}
	, m_refDeviceResources{ deviceResources }
{
}

// 初期化処理
void Renderings::DeviceResources2D::Initialize()
{
	// ファクトリの作成
	CreateFactory();
	// デバイスの作成
	CreateDevice();
	// バックバッファの作成
	CreateBackBuffer();
}

// リセット
void Renderings::DeviceResources2D::Reset()
{
	m_backBuffer.Reset();
}

// ウィンドウサイズ変更時の処理
void Renderings::DeviceResources2D::OnWindowSizeChanged()
{
	// バックバッファの作成
	CreateBackBuffer();
}

// ファクトリの作成
void Renderings::DeviceResources2D::CreateFactory()
{
	// ファクトリのオプション
	D2D1_FACTORY_OPTIONS options{};
#ifdef _DEBUG
	options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif // _DEBUG

	Utility::ThrowIfFailed(D2D1CreateFactory
	(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		__uuidof(ID2D1Factory8),
		&options,
		reinterpret_cast<void**>(m_d2dFactory.GetAddressOf())
	));
}

// デバイスの作成
void Renderings::DeviceResources2D::CreateDevice()
{
	// DXGIデバイス
	auto dxgiDevice = m_refDeviceResources.CreateDXGIDevice();

	Utility::ThrowIfFailed(m_d2dFactory->CreateDevice(dxgiDevice.Get(), m_d2dDevice.GetAddressOf()));
	Utility::ThrowIfFailed(m_d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, m_d2dContext.GetAddressOf()));
}

// バックバッファの作成
void Renderings::DeviceResources2D::CreateBackBuffer()
{
	// サーフェス
	Microsoft::WRL::ComPtr<IDXGISurface> surface;
	Utility::ThrowIfFailed(m_refDeviceResources.GetSwapChain()->GetBuffer(0, IID_PPV_ARGS(surface.GetAddressOf())));

	Utility::ThrowIfFailed(m_d2dContext->CreateBitmapFromDxgiSurface
	(
		surface.Get(),
		nullptr,
		m_backBuffer.ReleaseAndGetAddressOf()
	));
}
