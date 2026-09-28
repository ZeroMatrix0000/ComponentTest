#include "Pch.h"
#include "TextColorEffect.h"

// コンストラクタ
Renderings::TextColorEffect::TextColorEffect(const D2D1::ColorF& color)
	: m_color{ color }
{
}

HRESULT __stdcall Renderings::TextColorEffect::QueryInterface(REFIID riid, void** ppvObject)
{
	if (!ppvObject)
	{
		return E_POINTER;
	}

	*ppvObject = nullptr;

	if (riid == IID_IUnknown)
	{
		*ppvObject = static_cast<IUnknown*>(this);
		AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

// 参照解除されたときの処理
ULONG __stdcall Renderings::TextColorEffect::Release()
{
	ULONG count = --m_refCount;

	if (count == 0)
	{
		delete this;
	}

	return count;
}
