/*
 * FileName:     Spring.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/15
 *
 * ばね
 */

namespace Libraries
{
	// ばね
	template<typename T> requires IsSame
	<
		T,
		float,
		Math::Vector2,
		Math::Vector3,
		Math::Quaternion,
		Math::Euler,
		Camera::QuaternionCamera,
		Camera::QuaternionTargetCamera,
		Camera::EulerCamera,
		Camera::EulerTargetCamera
	>
	class Spring
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		Spring()
			: m_current{}
			, m_target{}
			, m_velocity{}
			, m_k{}
			, m_b{}
		{
		}

		// 更新処理
		void Tick(float elapsedTime)
		{
			// 移動量[m]
			T delta = m_target - m_current;

			// 加速度[m/s^2 ≒ kg･m/s^2 = N]
			T acceleration = delta * m_k - m_velocity * m_b;

			// 速度に加速度を足す
			m_velocity += acceleration * elapsedTime;
			// 位置に速度を足す
			m_current += m_velocity * elapsedTime;
		}

		// 瞬間的な値変化
		void SetValue(T value)
		{
			m_current = value;
			m_target = value;
			m_velocity = T{};
		}
		// 目的値を設定
		void SetTarget(T target, float k = 100.0f, float b = 20.0f)
		{
			m_target = target;
			m_k = k;
			m_b = b;
		}

		// 現在値を取得
		const T& GetCurrent() const { return m_current; }
		// 目標値を取得
		const T& GetTarget() const { return m_target; }


	private:


		/* メンバ変数 */

		// 現在値[m]
		T m_current;
		// 目標値[m]
		T m_target;

		// 速度[m/s]
		T m_velocity;

		// ばね定数[N/m]
		float m_k;
		// 比例定数[N･s/m]
		float m_b;

	};

	template<>
	class Spring<Math::Quaternion>
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		Spring()
			: m_current{}
			, m_target{}
			, m_velocity{}
			, m_k{}
			, m_b{}
		{
		}

		// 更新処理
		void Tick(float elapsedTime)
		{
			// 現在値の逆数
			Math::Quaternion currentInverse;
			m_current.Inverse(currentInverse);
			// 移動量[m]
			Math::Quaternion delta = currentInverse * m_target;

			// 最短経路にする
			if (delta.w < 0.0f)
			{
				delta *= -1.0f;
			}

			// 移動量の回転軸
			Math::Vector3 deltaAxis = Math::Vector3{ delta.x, delta.y, delta.z };
			deltaAxis.Normalize();
			// 移動量の回転角度
			float deltaAngle = Math::Arccos(Limited::Create(delta.w, -1.0f, 1.0f)) * 2.0f;
			if (Math::IsNaN(deltaAngle))
			{
				deltaAngle = 0.0f;
			}

			// 加速度[m/s^2 ≒ kg･m/s^2 = N]
			Math::Vector3 acceleration = deltaAxis * deltaAngle * m_k - m_velocity * m_b;

			// 速度に加速度を足す
			m_velocity += acceleration * elapsedTime;
			// 速度が極小なら何もしない
			if (m_velocity.Length() < Math::EPSILON)
			{
				return;
			}

			// 回転軸
			Math::Vector3 axis = m_velocity;
			axis.Normalize();
			// 回転角度
			float angle = m_velocity.Length() * elapsedTime;

			// 位置に速度を足す
			m_current *= Math::Quaternion::CreateFromAxisAngle(axis, angle);
			m_current.Normalize();
		}

		// 瞬間的な値変化
		void SetValue(const Math::Quaternion& value)
		{
			m_current = value;
			m_target = value;
			m_velocity = Math::Vector3::Zero;
		}
		// 目的値を設定
		void SetTarget(const Math::Quaternion& target, float k = 100.0f, float b = 20.0f)
		{
			m_target = target;
			m_k = k;
			m_b = b;
		}

		// 現在値を取得
		const Math::Quaternion& GetCurrent() const { return m_current; }
		// 目標値を取得
		const Math::Quaternion& GetTarget() const { return m_target; }


	private:


		/* メンバ変数 */

		// 現在値[m]
		Math::Quaternion m_current;
		// 目標値[m]
		Math::Quaternion m_target;

		// 速度[m/s]（回転の軸と角度を保持）
		Math::Vector3 m_velocity;

		// ばね定数[N/m]
		float m_k;
		// 比例定数[Ns/m]
		float m_b;

	};

	template<>
	inline void Spring<Math::Euler>::Tick(float elapsedTime)
	{
		// 移動量[m]
		Math::Euler delta = (m_target - m_current).Format();

		// 加速度[m/s^2 ≒ kg･m/s^2 = N]
		Math::Euler acceleration = acceleration = delta * m_k - m_velocity * m_b;

		// 速度に加速度を足す
		m_velocity += acceleration * elapsedTime;

		// 位置に速度を足す
		m_current = (m_current + m_velocity * elapsedTime).Format();
	}

	template<>
	class Spring<Camera::QuaternionCamera>
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		Spring()
			: m_position{}
			, m_rotation{}
		{
		}

		// 更新処理
		void Tick(float elapsedTime)
		{
			m_position.Tick(elapsedTime);
			m_rotation.Tick(elapsedTime);
		}

		// 瞬間的な値変化
		void SetValue(const Camera::QuaternionCamera& value)
		{
			m_position.SetValue(value.position);
			m_rotation.SetValue(value.rotation);
		}
		// 目的値を設定
		void SetTarget(const Camera::QuaternionCamera& target, float k = 100.0f, float b = 20.0f)
		{
			m_position.SetTarget(target.position, k, b);
			m_rotation.SetTarget(target.rotation, k, b);
		}

		// 現在値を取得
		const Camera::QuaternionCamera GetCurrent() const { return Camera::QuaternionCamera{ m_position.GetCurrent(), m_rotation.GetCurrent() }; }
		// 目標値を取得
		const Camera::QuaternionCamera GetTarget()  const { return Camera::QuaternionCamera{ m_position.GetTarget(), m_rotation.GetTarget() }; }


	private:


		/* メンバ変数 */

		// 位置
		Spring<Math::Vector3>    m_position;
		// 回転
		Spring<Math::Quaternion> m_rotation;

	};

	template<>
	class Spring<Camera::QuaternionTargetCamera>
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		Spring()
			: m_position{}
			, m_rotation{}
			, m_distance{}
		{
		}

		// 更新処理
		void Tick(float elapsedTime)
		{
			m_position.Tick(elapsedTime);
			m_rotation.Tick(elapsedTime);
			m_distance.Tick(elapsedTime);
		}

		// 瞬間的な値変化
		void SetValue(const Camera::QuaternionTargetCamera& value)
		{
			m_position.SetValue(value.position);
			m_rotation.SetValue(value.rotation);
			m_distance.SetValue(value.distance);
		}
		// 目的値を設定
		void SetTarget(const Camera::QuaternionTargetCamera& target, float k = 100.0f, float b = 20.0f)
		{
			m_position.SetTarget(target.position, k, b);
			m_rotation.SetTarget(target.rotation, k, b);
			m_distance.SetTarget(target.distance, k, b);
		}

		// 現在値を取得
		const Camera::QuaternionTargetCamera GetCurrent() const
		{
			return Camera::QuaternionTargetCamera
			{
				m_position.GetCurrent(),
				m_rotation.GetCurrent(),
				m_distance.GetCurrent()
			};
		}
		// 目標値を取得
		const Camera::QuaternionTargetCamera GetTarget() const
		{
			return Camera::QuaternionTargetCamera
			{
				m_position.GetTarget(),
				m_rotation.GetTarget(),
				m_distance.GetTarget()
			};
		}


	private:


		/* メンバ変数 */

		// 位置
		Spring<Math::Vector3>    m_position;
		// 回転
		Spring<Math::Quaternion> m_rotation;
		// ターゲットの距離
		Spring<float>            m_distance;

	};

	template<>
	class Spring<Camera::EulerCamera>
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		Spring()
			: m_position{}
			, m_rotation{}
		{
		}

		// 更新処理
		void Tick(float elapsedTime)
		{
			m_position.Tick(elapsedTime);
			m_rotation.Tick(elapsedTime);
		}

		// 瞬間的な値変化
		void SetValue(const Camera::EulerCamera& value)
		{
			m_position.SetValue(value.position);
			m_rotation.SetValue(value.rotation);
		}
		// 目的値を設定
		void SetTarget(const Camera::EulerCamera& target, float k = 100.0f, float b = 20.0f)
		{
			m_position.SetTarget(target.position, k, b);
			m_rotation.SetTarget(target.rotation, k, b);
		}

		// 現在値を取得
		const Camera::EulerCamera GetCurrent() const { return Camera::EulerCamera{ m_position.GetCurrent(), m_rotation.GetCurrent() }; }
		// 目標値を取得
		const Camera::EulerCamera GetTarget()  const { return Camera::EulerCamera{ m_position.GetTarget(), m_rotation.GetTarget() }; }


	private:


		/* メンバ変数 */

		// 位置
		Spring<Math::Vector3> m_position;
		// 回転
		Spring<Math::Euler>   m_rotation;

	};

	template<>
	class Spring<Camera::EulerTargetCamera>
	{

	public:


		/* メンバ関数 */

		// コンストラクタ
		Spring()
			: m_position{}
			, m_rotation{}
			, m_distance{}
		{
		}

		// 更新処理
		void Tick(float elapsedTime)
		{
			m_position.Tick(elapsedTime);
			m_rotation.Tick(elapsedTime);
			m_distance.Tick(elapsedTime);
		}

		// 瞬間的な値変化
		void SetValue(const Camera::EulerTargetCamera& value)
		{
			m_position.SetValue(value.position);
			m_rotation.SetValue(value.rotation);
			m_distance.SetValue(value.distance);
		}
		// 目的値を設定
		void SetTarget(const Camera::EulerTargetCamera& target, float k = 100.0f, float b = 20.0f)
		{
			m_position.SetTarget(target.position, k, b);
			m_rotation.SetTarget(target.rotation, k, b);
			m_distance.SetTarget(target.distance, k, b);
		}

		// 現在値を取得
		const Camera::EulerTargetCamera GetCurrent() const
		{
			return Camera::EulerTargetCamera
			{
				m_position.GetCurrent(),
				m_rotation.GetCurrent(),
				m_distance.GetCurrent()
			};
		}
		// 目標値を取得
		const Camera::EulerTargetCamera GetTarget() const
		{
			return Camera::EulerTargetCamera
			{
				m_position.GetTarget(),
				m_rotation.GetTarget(),
				m_distance.GetTarget()
			};
		}


	private:


		/* メンバ変数 */

		// 位置
		Spring<Math::Vector3> m_position;
		// 回転
		Spring<Math::Euler>   m_rotation;
		// ターゲットの距離
		Spring<float>         m_distance;

	};
}
