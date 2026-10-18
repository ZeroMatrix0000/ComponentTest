/*
 * FileName:     Geometry.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/15
 *
 * 幾何学関係
 */

#pragma once

namespace Libraries
{
	namespace Math
	{
		// 幾何学関係
		namespace Geometry
		{
			// 3次元上の座標を2次元に投影
			Vector2 Projection
			(
				const Vector3&    position,
				const Matrix&     view,
				const Matrix&     proj,
				const Vector2Int& outputSize
			);
			// スクリーン座標をワールドに移す
			Vector3 ToWorldPosition
			(
				const Vector2&    position,
				float             depth,
				const Matrix&     view,
				const Matrix&     proj,
				const Vector2Int& outputSize
			);

			// 長方形と点が触れているかどうか
			bool IsCollide(const Rect& rect, const Vector2& position);
			// 非回転直方体と点が触れているかどうか
			bool IsCollide(const NonRotatingBox& box, const Vector3& position);
			// 非回転直方体同士が触れているかどうか
			bool IsCollide(const NonRotatingBox& box1, const NonRotatingBox& box2);
			// 直方体同士が触れているかどうか
			bool IsCollide(const Box& box1, const Box& box2);
			// 線分と三角形が触れているかどうか
			bool IsCollide(const Line& line, const Triangle& triangle);

			// 点と球の距離
			inline float Distance(const Vector3& point, const Sphere& sphere) { return Vector3::Distance(sphere.centerPosition, point) - sphere.radius; }
			// 点と線分の距離
			       float Distance(const Vector3& point, const Line& line);
			// 球と線分の距離
			inline float Distance(const Sphere& sphere, const Line& line) { return Distance(sphere.centerPosition, line) - sphere.radius; };
			// 点と平面の距離
			inline float Distance(const Vector3& point, const Plane& plane) { return Math::Abs(plane.Normal().Dot(point) + plane.D()) / plane.Normal().Length(); }
			// 直方体同士の距離
			       float Distance(const Box& box1, const Box& box2);
			// 直方体同士の距離
			       float Distance(const Box& box1, const Box& box2, Vector3* pDirection);
			// 三角形と直方体の距離
			       float Distance(const Triangle& triangle, const Box& box, Vector3* pDirection, bool enableNormal);

			// 点から球への方向指定最短距離
			std::pair<float, float> AngledDistance(const Vector3& from, const Sphere& to, const Vector3& direction);
			// 点から三角形への方向指定最短距離
			float AngledDistance(const Vector3& from, const Triangle& to, const Vector3& direction);
			// 線分同士の方向指定最短距離
			float AngledDistance(const Line& from, const Line& to, const Vector3& direction);
			// 点から非回転直方体への方向指定最短距離
			float AngledDistance(const Vector3& from, const NonRotatingBox& to, const Vector3& direction);
			// 点から直方体への方向指定最短距離
			float AngledDistance(const Vector3& from, const Box& to, const Vector3& direction);
			// 直方体から三角形への方向指定最短距離
			float AngledDistance(const Box& from, const Triangle& to, const Vector3& direction);
		}
	}
}
