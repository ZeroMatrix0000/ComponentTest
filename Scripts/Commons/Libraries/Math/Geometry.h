/*
 * FileName:     Geometry.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/14
 *
 * 数学関係
 */

#pragma once

namespace Libraries
{
	namespace Math
	{
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

			// 平面と点の距離
			inline float Distance(const Vector3& point, const Plane& plane) { return Math::Abs(plane.Normal().Dot(point) + plane.D()) / plane.Normal().Length(); }
			// 球と点の距離
			inline float Distance(const Vector3& point, const Sphere& sphere) { return (sphere.centerPosition - point).Length() - sphere.radius; }
			// 直方体同士の距離
			       float Distance(const Box& box1, const Box& box2);
			// 直方体同士の距離
			       float Distance(const Box& box1, const Box& box2, Vector3* pDirection);
			// 三角形と直方体の距離
			       float Distance(const Triangle& triangle, const Box& box, Vector3* pDirection);

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
