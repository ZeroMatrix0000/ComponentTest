/*
 * FileName:     CameraScreen.h
 * Author:       Takao Hayata
 * Last Updated: 2026/09/04
 *
 * カメラ画面
 */

#pragma once

#include "ICameraScreen.h"
#include "Model3D/IModel3DRenderer.h"
#include "../Systems/JsonSerializer.h"

namespace Renderings
{
	// カメラ画面
	template<typename TCamera> requires IsSame
	<
		TCamera,
		Camera::QuaternionCamera,
		Camera::QuaternionTargetCamera,
		Camera::EulerCamera,
		Camera::EulerTargetCamera
	>
	class CameraScreen : public ICameraScreen
	{

	public:


		// 親コンポーネントの定義
		using Base = ICameraScreen;


		/* メンバ関数 */

		// コンストラクタ
		CameraScreen(const ComponentDesc& desc)
			: ICameraScreen{ desc }
			, m_camera{}
			, m_viewAngle{}
			, m_view{}
			, m_projection{}
			, m_outputSize{}
		{
		}

		// 初期化処理
		void Initalize(const nlohmann::ordered_json& json, IGameObjectFinder* pIGameObjectFinder) override
		{
			Systems::JsonSerializer serializer{ pIGameObjectFinder };
			serializer.AddParameter(&m_camera, "Camera");
			serializer.AddParameter(&m_viewAngle, "ViewAngle");
			serializer.Load(json);
		}

		// ビュー行列を更新
		void UpdateViewMatrix() override
		{
			m_view = m_camera.CreateViewMatrix();
		}

		// 座標を設定
		void SetPosition(const Math::Vector3& position) override
		{
			m_camera.position = position;
		}

		// プロジェクション行列を設定
		void SetProjectionMatrix(const Math::Vector2& outputSize) override
		{
			if (m_viewAngle == 0.0f)
			{
				m_projection = Math::Matrix::Identity;
				return;
			}

			m_projection = Math::Matrix::CreatePerspectiveFieldOfView
			(
				Math::Deg2Rad(m_viewAngle),
				outputSize.x / outputSize.y,
				0.1f,
				1000.0f
			);

			m_outputSize = outputSize;
		}

		// ビュー行列を取得
		const Math::Matrix& GetViewMatrix() const override { return m_view; }
		// プロジェクション行列を取得
		const Math::Matrix& GetProjectionMatrix() const override { return m_projection; }

		// 目の座標を取得
		const Math::Vector3 GetEyePosition() const override { return m_camera.GetEyePosition(); }

		// 出力サイズを取得
		const Math::Vector2& GetOutputSize() const override { return m_outputSize; }

		// カメラを設定
		void SetCamera(const TCamera& camera) { m_camera = camera; }
		// カメラを設定
		void SetViewAngle(float viewAngle) { m_viewAngle = viewAngle; }

		// カメラを取得
		const TCamera& GetCamera() const { return m_camera; }


	private:


		/* メンバ変数 */

		// カメラ
		TCamera m_camera;

		// 視野角
		float m_viewAngle;

		// ビュー行列
		Math::Matrix m_view;
		// プロジェクション行列
		Math::Matrix m_projection;

		// 出力サイズ
		Math::Vector2 m_outputSize;

	};
}
