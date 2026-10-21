/*
 * FileName:     JsonSerializer.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/21
 *
 * Jsonのシリアライズ
 */

#pragma once

#include "../GameObjects/IGameObjectFinder.h"
#include "../GameObjects/GameObject.h"

namespace Components
{
	class Component;
}

namespace Systems
{
	// Jsonのシリアライズ
	class JsonSerializer
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		JsonSerializer(IGameObjectFinder* pIGameObjectFinder);

		// パラメータを追加
		template<typename T>
		void AddParameter(T* ptr, const std::string& name)
		{
			// パラメータを追加
			m_parameters.emplace(name, Parameter{ ptr, typeid(T) });

			// 関数を探す
			auto it = s_methods.find(typeid(T));
			if (it == s_methods.end())
			{
				s_methods.emplace(typeid(T), [](void* ptr, const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder) { Serialize(static_cast<T*>(ptr), json, pIGameObjectFinder); });
			}
		}

		// パラメータを読み込む
		void Load(const nlohmann::ordered_json& json);


	private:


		/* 構造体 */

		// パラメータ
		struct Parameter
		{
			// パラメータのポインタ
			void* ptr;
			// 型インデックス
			std::type_index index;
		};


		/* 静的関数 */


		// シリアライズ
		template<typename T>
		static void Serialize(T* ptr, const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder)
		{
			if constexpr (requires{ json.get<T>(); })
			{
				*ptr = json.get<T>();
				return;
			}
		}
		// シリアライズ（列挙型）
		template<typename TEnum> requires std::is_enum_v<TEnum>
		static void Serialize(TEnum* ptr, const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder)
		{
			*ptr = magic_enum::enum_cast<TEnum>(json.get<std::string>()).value();
		}
		// シリアライズ（std::wstring）
		template<>
		static void Serialize(std::wstring* ptr, const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder)
		{
			*ptr = Utility::string2wstring(json.get<std::string>());
		}
		// シリアライズ（D2D1::ColorF）
		template<>
		static void Serialize(D2D1::ColorF* ptr, const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder)
		{
			*ptr = D2D1::ColorF
			{
				json.at("R").get<float>(),
				json.at("G").get<float>(),
				json.at("B").get<float>(),
				json.at("A").get<float>()
			};
		}
		// シリアライズ（Math::Color）
		template<>
		static void Serialize(Math::Color* ptr, const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder)
		{
			*ptr = Math::Color
			{
				json.at("R").get<float>(),
				json.at("G").get<float>(),
				json.at("B").get<float>(),
				json.at("A").get<float>()
			};
		}
		// シリアライズ（Math::Vector2）
		template<>
		static void Serialize(Math::Vector2* ptr, const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder)
		{
			*ptr = Math::Vector2
			{
				json.at("X").get<float>(),
				json.at("Y").get<float>()
			};
		}
		// シリアライズ（Math::Vector3）
		template<>
		static void Serialize(Math::Vector3* ptr, const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder)
		{
			*ptr = Math::Vector3
			{
				json.at("X").get<float>(),
				json.at("Y").get<float>(),
				json.at("Z").get<float>()
			};
		}
		// シリアライズ（Math::Euler）
		template<>
		static void Serialize(Math::Euler* ptr, const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder)
		{
			*ptr = Math::Euler
			{
				json.at("X").get<float>(),
				json.at("Y").get<float>(),
				json.at("Z").get<float>()
			};
		}
		// シリアライズ（Math::Quaternion）
		template<>
		static void Serialize(Math::Quaternion* ptr, const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder)
		{
			*ptr = Math::Euler
			{
				json.at("X").get<float>(),
				json.at("Y").get<float>(),
				json.at("Z").get<float>()
			}.CreateQuaternion();
		}
		// シリアライズ（Camera::QuaternionCamera）
		template<>
		static void Serialize(Camera::QuaternionCamera* ptr, const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder)
		{
			Serialize(&ptr->position, json.at("Position"), pIGameObjectFinder);
			Serialize(&ptr->rotation, json.at("Rotation"), pIGameObjectFinder);
		}
		// シリアライズ（Camera::QuaternionTargetCamera）
		template<>
		static void Serialize(Camera::QuaternionTargetCamera* ptr, const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder)
		{
			Serialize(&ptr->position, json.at("Position"), pIGameObjectFinder);
			Serialize(&ptr->rotation, json.at("Rotation"), pIGameObjectFinder);
			Serialize(&ptr->distance, json.at("Distance"), pIGameObjectFinder);
		}
		// シリアライズ（Camera::EulerCamera）
		template<>
		static void Serialize(Camera::EulerCamera* ptr, const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder)
		{
			Serialize(&ptr->position, json.at("Position"), pIGameObjectFinder);
			Serialize(&ptr->rotation, json.at("Rotation"), pIGameObjectFinder);
		}
		// シリアライズ（Camera::EulerTargetCamera）
		template<>
		static void Serialize(Camera::EulerTargetCamera* ptr, const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder)
		{
			Serialize(&ptr->position, json.at("Position"), pIGameObjectFinder);
			Serialize(&ptr->rotation, json.at("Rotation"), pIGameObjectFinder);
			Serialize(&ptr->distance, json.at("Distance"), pIGameObjectFinder);
		}

		// シリアライズ（コンポーネント）
		template<typename TComponent> requires IsDerived<TComponent, Component>
		static void Serialize(TComponent** ptr, const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder)
		{
			*ptr = &pIGameObjectFinder->Find(json.get<std::string>()).GetComponent<TComponent>();
		}
		// シリアライズ（変更負荷コンポーネント）
		template<typename TComponent> requires IsDerived<TComponent, Component>
		static void Serialize(const TComponent** ptr, const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder)
		{
			*ptr = &pIGameObjectFinder->Find(json.get<std::string>()).GetConstComponent<TComponent>();
		}

		// シリアライズ（コンポーネント）
		template<typename TComponent> requires IsDerived<TComponent, Component>
		static void Serialize(Ref<TComponent>* ptr, const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder)
		{
			*ptr = pIGameObjectFinder->Find(json.get<std::string>()).GetComponent<TComponent>();
		}
		// シリアライズ（変更負荷コンポーネント）
		template<typename TComponent> requires IsDerived<TComponent, Component>
		static void Serialize(Ref<const TComponent>* ptr, const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder)
		{
			*ptr = pIGameObjectFinder->Find(json.get<std::string>()).GetConstComponent<TComponent>();
		}

		// シリアライズ（std::vector）
		template<typename T>
		static void Serialize(std::vector<T>* ptr, const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder)
		{
			ptr->clear();
			for (const auto& element : json)
			{
				ptr->push_back({});
				Serialize(&ptr->back(), element, pIGameObjectFinder);
			}
		}

		// シリアライズ（std::vector）
		template<typename T>
		static void Serialize(std::vector<Ref<T>>* ptr, const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder)
		{
			ptr->clear();
			for (const auto& element : json)
			{
				T* value{};
				Serialize(&value, element, pIGameObjectFinder);
				ptr->push_back(*value);
			}
		}


		/* 静的変数 */

		// パラメータを設定する関数リスト
		static std::unordered_map<std::type_index, std::function<void(void*, const nlohmann::ordered_json&, IGameObjectFinder*)>> s_methods;


		/* メンバ変数 */

		// パラメータリスト
		std::unordered_map<std::string, Parameter> m_parameters;

		// ゲームオブジェクト検索インタフェースのポインタ
		IGameObjectFinder* m_pIGameObjectFinder;

	};
}
