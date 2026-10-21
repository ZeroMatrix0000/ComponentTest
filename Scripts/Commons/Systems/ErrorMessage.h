/*
 * FileName:     ErrorMessage.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/21
 *
 * エラーメッセージ
 */

#pragma once

#include "IErrorMessage.h"

namespace Renderings
{
	class Text;
}
namespace GameObjects
{
	class GameObject;
	class IGameObjectInstantiator;
}

namespace Systems
{
	// エラーメッセージ
	class ErrorMessage : public IErrorMessage
	{

	public:

		/* メンバ関数 */


		// コンストラクタ
		ErrorMessage();

		// 初期化処理
		void Initialize(float displayTime, size_t displayMax, bool isActive);
		// 更新処理
		void Update(float elapsedTime);

		// オブジェクト生成
		void CreateObjects(IGameObjectInstantiator* pIGameObjectInstantiator);

		// メッセージを追加
		void AddMessage(const std::wstring& text) override;


	private:


		/* 構造体 */

		struct MessageData
		{
			// テキスト
			std::wstring text{};
			// 表示時間
			Limited timer{};
		};


		/* メンバ関数 */

		// 表示を適用
		void ApplyDisplayText();


		/* メンバ変数 */

		// メッセージリスト
		std::vector<MessageData> m_messages;

		// 表示するテキストコンポーネント
		Renderings::Text* m_pText;

		// 表示する秒数
		float  m_displayTime;
		// 表示する最大数
		size_t m_displayMax;

		// アクティブフラグ
		bool m_isActive;

		// 変更があったか
		bool m_isChanged;

	};
}
