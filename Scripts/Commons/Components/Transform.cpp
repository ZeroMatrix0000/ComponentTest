/*
 * FileName:     Transform.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/07
 *
 * トランスフォーム
 */

#include "Pch.h"
#include "Transform.h"

#include "../Systems/JsonSerializer.h"

Components::Transform::Transform(const ComponentDesc& desc)
	: Component{ desc }
	, m_position{ Math::Vector3::Zero }
	, m_rotation{ Math::Quaternion::Identity }
	, m_scale{ Math::Vector3::One }
	, m_matrix{}
	, m_isChanged{}
{
}

// 初期化処理
void Components::Transform::Initalize(const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder)
{
	Systems::JsonSerializer serializer{ pIGameObjectFinder };
	serializer.AddParameter(&m_position, "Position");
	serializer.AddParameter(&m_rotation, "Rotation");
	serializer.AddParameter(&m_scale, "Scale");
	serializer.Load(json);

	UpdateWorldMatrix();
}

// 座標を設定
void Components::Transform::SetPosition(const Math::Vector3& position)
{
	m_position = position;
	m_isChanged = true;
}

// 回転を設定
void Components::Transform::SetRotation(const Math::Quaternion& rotation)
{
	m_rotation = rotation;
	m_isChanged = true;
}

// 拡大を設定
void Components::Transform::SetScale(const Math::Vector3& scale)
{
	m_scale = scale;
	m_isChanged = true;
}

// 座標を足す
void Components::Transform::Translate(const Math::Vector3& displacement)
{
	m_position += displacement;
	m_isChanged = true;
}

// ワールド行列を作成
const Math::Matrix& Components::Transform::GetWorldMatrix() const
{
	if (m_isChanged)
	{
		m_matrix =
			Math::Matrix::CreateScale(m_scale) *
			Math::Matrix::CreateFromQuaternion(m_rotation) *
			Math::Matrix::CreateTranslation(m_position)
		;
		m_isChanged = false;
	}
	return m_matrix;
}

// 行列を更新
void Components::Transform::UpdateWorldMatrix()
{
	m_matrix =
		Math::Matrix::CreateScale(m_scale) *
		Math::Matrix::CreateFromQuaternion(m_rotation) *
		Math::Matrix::CreateTranslation(m_position)
	;
}
