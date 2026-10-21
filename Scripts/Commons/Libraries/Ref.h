/*
 * FileName:     Ref.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/21
 *
 * 自作参照型
 */

#pragma once

namespace Libraries
{
	// 自作参照型（参照先変更可能・nullptr禁止）
	template<typename T>
	class Ref
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		Ref(T& ref)
			: m_ptr{ &ref }
		{
		}

		// 生ポインタの取得
		T* Get() const { return m_ptr; }

		
		/* 演算子オーバーロード */

		T* operator->() const { return m_ptr; }

		Ref& operator=(T& ref)
		{
			m_ptr = &ref;
			return *this;
		}

		operator T& () const { return *m_ptr; }


	private:


		/* メンバ変数 */

		// ポインタ
		T* m_ptr;

	};

	template<typename T>
	bool operator==(const Ref<T>& a, const T& b) { return a.Get() == &b; }

	template<typename T>
	bool operator==(const T& a, const Ref<T>& b) { return &a == b.Get(); }
}
