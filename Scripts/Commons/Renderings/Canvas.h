/*
 * FileName:     Canvas.h
 * Author:       Takao Hayata
 * Last Updated: 2026/09/11
 *
 * カメラ画面
 */

#pragma once

#include "../Components/Component.h"

namespace Renderings
{
	// カメラ画面
	class Canvas : public Component
	{

	public:


		/* 列挙型 */

		// キャンバスサイズの設定方法
		enum class FixedSize
		{
			None,
			Horizontal,
			Vertical
		};


		/* メンバ関数 */

		// コンストラクタ
		Canvas(const ComponentDesc& desc);

		// 初期化処理
		void Initalize(const nlohmann::ordered_json& json, Ref<IGameObjectFinder> iGameObjectFinderRef) override;
		// 初期化処理
		void Initialize(FixedSize fixedSize, const Math::Vector2& outputSize);
		
		// サイズを設定
		void SetSize(const Math::Vector2& outputSize);

		// キャンバスのサイズを取得
		const Math::Vector2& GetSize() const { return m_size; }
		// キャンバスの表示倍率を取得
		float GetRatio() const { return m_ratio; }


	private:


		/* 定数 */

		// キャンバスのデフォルトサイズ
		static constexpr Math::Vector2 DEFAULT_SIZE = Math::Vector2{ 1600.0f, 900.0f };


		/* メンバ変数 */

		// キャンバスサイズの設定方法
		FixedSize m_fixedSize;

		// キャンバスサイズ
		Math::Vector2 m_size;
		// キャンバスの表示倍率
		float m_ratio;

	};
}
