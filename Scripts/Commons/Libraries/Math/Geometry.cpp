/*
 * FileName:     Geometry.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/14
 *
 * 数学関係
 */

#include "Pch.h"

 // 3次元上の座標を2次元に投影
Math::Vector2 Libraries::Math::Geometry::Projection
(
	const Vector3&    position,
	const Matrix&     view,
	const Matrix&     proj,
	const Vector2Int& outputSize
)
{
	// ビューポート
	Math::Viewport viewPort{ 0.0f, 0.0f, static_cast<float>(outputSize.x), static_cast<float>(outputSize.y) };
	// スクリーン座標
	Vector3 screenPosition = viewPort.Project(position, proj, view, Matrix::Identity);
	return Vector2{ screenPosition.x, screenPosition.y };
}

// スクリーン座標をワールドに移す
Math::Vector3 Libraries::Math::Geometry::ToWorldPosition
(
	const Vector2&    position,
	float             depth,
	const Matrix&     view,
	const Matrix&     proj,
	const Vector2Int& outputSize
)
{
	// ビューポート
	Math::Viewport viewPort{ 0.0f, 0.0f, static_cast<float>(outputSize.x), static_cast<float>(outputSize.y) };
	return viewPort.Unproject(Vector3{ position.x, position.y, depth }, proj, view, Matrix::Identity);
}

// 長方形と点が触れているかどうか
bool Libraries::Math::Geometry::IsCollide(const Rect& rect, const Vector2& position)
{
	return
		position.x > rect.position.x - rect.size.x / 2.0f &&
		position.y > rect.position.y - rect.size.y / 2.0f &&
		position.x < rect.position.x + rect.size.x / 2.0f &&
		position.y < rect.position.y + rect.size.y / 2.0f
	;
}

// 非回転直方体と点が触れているかどうか
bool Libraries::Math::Geometry::IsCollide(const NonRotatingBox& box, const Vector3& position)
{
	return
		position.x > box.position.x - box.size.x / 2.0f &&
		position.y > box.position.y - box.size.y / 2.0f &&
		position.z > box.position.z - box.size.z / 2.0f &&
		position.x < box.position.x + box.size.x / 2.0f &&
		position.y < box.position.y + box.size.y / 2.0f &&
		position.z < box.position.z + box.size.z / 2.0f
	;
}

// 非回転直方体同士が触れているかどうか
bool Libraries::Math::Geometry::IsCollide(const NonRotatingBox& box1, const NonRotatingBox& box2)
{
	return
		Math::Abs((box1.position - box2.position).x) < (box1.size + box2.size).x / 2.0f &&
		Math::Abs((box1.position - box2.position).y) < (box1.size + box2.size).y / 2.0f &&
		Math::Abs((box1.position - box2.position).z) < (box1.size + box2.size).z / 2.0f
	;
}

// 直方体同士が触れているかどうか
bool Libraries::Math::Geometry::IsCollide(const Box& box1, const Box& box2)
{
	//SAT法

	//分離軸
	Vector3 axes[15];
	axes[ 0] = Vector3::Transform(Vector3::UnitX, box1.rotation);
	axes[ 1] = Vector3::Transform(Vector3::UnitY, box1.rotation);
	axes[ 2] = Vector3::Transform(Vector3::UnitZ, box1.rotation);
	axes[ 3] = Vector3::Transform(Vector3::UnitX, box2.rotation);
	axes[ 4] = Vector3::Transform(Vector3::UnitY, box2.rotation);
	axes[ 5] = Vector3::Transform(Vector3::UnitZ, box2.rotation);
	axes[ 6] = axes[0].Cross(axes[3]);
	axes[ 7] = axes[0].Cross(axes[4]);
	axes[ 8] = axes[0].Cross(axes[5]);
	axes[ 9] = axes[1].Cross(axes[3]);
	axes[10] = axes[1].Cross(axes[4]);
	axes[11] = axes[1].Cross(axes[5]);
	axes[12] = axes[2].Cross(axes[3]);
	axes[13] = axes[2].Cross(axes[4]);
	axes[14] = axes[2].Cross(axes[5]);

	// 軸ごとに判定
	for (auto& axis : axes)
	{
		// 軸の長さが極小ならスキップ
		if (axis.Length() < Math::EPSILON)
		{
			continue;
		}
		// 正規化
		axis.Normalize();

		// 中心間の距離
		float centerDistance = Math::Abs((box1.position - box2.position).Dot(axis));
		// 直方体1の大きさ
		float size1 =
		(
			Math::Abs(axes[0].Dot(axis) * box1.size.x) +
			Math::Abs(axes[1].Dot(axis) * box1.size.y) +
			Math::Abs(axes[2].Dot(axis) * box1.size.z)
		) / 2.0f;
		// 直方体2の大きさ
		float size2 =
		(
			Math::Abs(axes[3].Dot(axis) * box2.size.x) +
			Math::Abs(axes[4].Dot(axis) * box2.size.y) +
			Math::Abs(axes[5].Dot(axis) * box2.size.z)
		) / 2.0f;

		if (size1 + size2 < centerDistance)
		{
			return false;
		}
	}

	return true;
}

// 線分と三角形が触れているかどうか
bool Libraries::Math::Geometry::IsCollide(const Line& line, const Triangle& triangle)
{
	// 三角形を含む平面
	Plane plane{ triangle.v1, triangle.v2, triangle.v3 };

	Vector3 v = line.v2 - line.v1;

	/*
	 * p = line.v1
	 * v = line.v2 - line.v1
	 * q = p + tv // 交点
	 *
	 * ax + by + cz + d = 0 // plane
	 * a = (a, b, c)
	 * a･q = -d
	 *
	 * -d = a･p + ta･v
	 * t = -(a･p + d) / a･v
	 */

	 // レイの倍率
	float t = -(plane.Normal().Dot(line.v1) + plane.D()) / plane.Normal().Dot(v);

	if (t < 0.0f || t > 1.0f)
	{
		return false;
	}

	// 公差位置
	Vector3 intersection = line.v1 + v * t;

	Vector3 normal1 = (triangle.v1 - triangle.v2).Cross(triangle.v1 - intersection);
	Vector3 normal2 = (triangle.v2 - triangle.v3).Cross(triangle.v2 - intersection);
	Vector3 normal3 = (triangle.v3 - triangle.v1).Cross(triangle.v3 - intersection);

	// いずれかの法線方向が逆なら公差位置は三角形の外
	if (Math::Sign(normal1.Dot(normal2)) == -1 || Math::Sign(normal2.Dot(normal3)) == -1)
	{
		return false;
	}
	return true;
}

// 点と線分の距離
float Libraries::Math::Geometry::Distance(const Vector3& point, const Line& line)
{
	Vector3 v = line.v2 - line.v1;

	/*
	 * a = line.v1
	 * v = line.v2 - line.v1
	 * p = point
	 * 
	 * q = a + tv
	 * (q - p)･v = 0
	 * 
	 * q - p = a - p + tv // これの長さが点と線分の距離
	 * 0 = (a - p)･v + t|v|^2
	 * t = (p - a)･v / |v|^2
	 */

	float t = v.Dot(point - line.v1) / v.LengthSquared();

	if (t < 0.0f || t > 1.0f)
	{
		return Math::Min(Vector3::Distance(line.v1, point), Vector3::Distance(line.v2, point));
	}

	return (line.v1 - point + t * v).Length();
}

// 直方体同士の距離
float Libraries::Math::Geometry::Distance(const Box& box1, const Box& box2)
{
	//SAT法

	//分離軸
	Vector3 axes[15];
	axes[ 0] = Vector3::Transform(Vector3::UnitX, box1.rotation);
	axes[ 1] = Vector3::Transform(Vector3::UnitY, box1.rotation);
	axes[ 2] = Vector3::Transform(Vector3::UnitZ, box1.rotation);
	axes[ 3] = Vector3::Transform(Vector3::UnitX, box2.rotation);
	axes[ 4] = Vector3::Transform(Vector3::UnitY, box2.rotation);
	axes[ 5] = Vector3::Transform(Vector3::UnitZ, box2.rotation);
	axes[ 6] = axes[0].Cross(axes[3]);
	axes[ 7] = axes[0].Cross(axes[4]);
	axes[ 8] = axes[0].Cross(axes[5]);
	axes[ 9] = axes[1].Cross(axes[3]);
	axes[10] = axes[1].Cross(axes[4]);
	axes[11] = axes[1].Cross(axes[5]);
	axes[12] = axes[2].Cross(axes[3]);
	axes[13] = axes[2].Cross(axes[4]);
	axes[14] = axes[2].Cross(axes[5]);

	float distanceMax = -Math::INFTY;

	// 軸ごとに判定
	for (auto& axis : axes)
	{
		// 軸の長さが極小ならスキップ
		if (axis.Length() < Math::EPSILON)
		{
			continue;
		}
		// 正規化
		axis.Normalize();

		// 中心間の距離
		float centerDistance = Math::Abs((box1.position - box2.position).Dot(axis));
		// 直方体1の大きさ
		float size1 =
		(
			Math::Abs(axes[0].Dot(axis) * box1.size.x) +
			Math::Abs(axes[1].Dot(axis) * box1.size.y) +
			Math::Abs(axes[2].Dot(axis) * box1.size.z)
		) / 2.0f;
		// 直方体2の大きさ
		float size2 =
		(
			Math::Abs(axes[3].Dot(axis) * box2.size.x) +
			Math::Abs(axes[4].Dot(axis) * box2.size.y) +
			Math::Abs(axes[5].Dot(axis) * box2.size.z)
		) / 2.0f;

		float distance = centerDistance - size1 - size2;
		if (distance > distanceMax)
		{
			distanceMax = distance;
		}
	}

	return distanceMax;
}

// 直方体同士の距離
float Libraries::Math::Geometry::Distance(const Box& box1, const Box& box2, Vector3* pDirection)
{
	//SAT法
	
	//分離軸
	Vector3 axes[15];
	axes[ 0] = Vector3::Transform(Vector3::UnitX, box1.rotation);
	axes[ 1] = Vector3::Transform(Vector3::UnitY, box1.rotation);
	axes[ 2] = Vector3::Transform(Vector3::UnitZ, box1.rotation);
	axes[ 3] = Vector3::Transform(Vector3::UnitX, box2.rotation);
	axes[ 4] = Vector3::Transform(Vector3::UnitY, box2.rotation);
	axes[ 5] = Vector3::Transform(Vector3::UnitZ, box2.rotation);
	axes[ 6] = axes[0].Cross(axes[3]);
	axes[ 7] = axes[0].Cross(axes[4]);
	axes[ 8] = axes[0].Cross(axes[5]);
	axes[ 9] = axes[1].Cross(axes[3]);
	axes[10] = axes[1].Cross(axes[4]);
	axes[11] = axes[1].Cross(axes[5]);
	axes[12] = axes[2].Cross(axes[3]);
	axes[13] = axes[2].Cross(axes[4]);
	axes[14] = axes[2].Cross(axes[5]);

	float distanceMax = -Math::INFTY;
	Vector3 maxAxis{};

	// 軸ごとに判定
	for (auto& axis : axes)
	{
		// 軸の長さが極小ならスキップ
		if (axis.Length() < Math::EPSILON)
		{
			continue;
		}
		// 正規化
		axis.Normalize();

		// 中心間の距離
		float centerDistance = Math::Abs((box1.position - box2.position).Dot(axis));
		// 直方体1の大きさ
		float size1 =
		(
			Math::Abs(axes[0].Dot(axis) * box1.size.x) +
			Math::Abs(axes[1].Dot(axis) * box1.size.y) +
			Math::Abs(axes[2].Dot(axis) * box1.size.z)
		) / 2.0f;
		// 直方体2の大きさ
		float size2 =
		(
			Math::Abs(axes[3].Dot(axis) * box2.size.x) +
			Math::Abs(axes[4].Dot(axis) * box2.size.y) +
			Math::Abs(axes[5].Dot(axis) * box2.size.z)
		) / 2.0f;

		float distance = centerDistance - size1 - size2;
		if (distance > distanceMax)
		{
			distanceMax = distance;
			maxAxis = axis;
		}
	}

	if ((box1.position - box2.position).Dot(maxAxis) >= 0.0f)
	{
		maxAxis *= -1.0f;
	}

	*pDirection = maxAxis;
	return distanceMax;
}

// 三角形と直方体の距離
float Libraries::Math::Geometry::Distance(const Triangle& triangle, const Box& box, Vector3* pDirection)
{
	//SAT法
	
	//分離軸
	Vector3 axes[13];
	axes[ 0] = triangle.GetNormal();
	axes[ 1] = Vector3::Transform(Vector3::UnitX, box.rotation);
	axes[ 2] = Vector3::Transform(Vector3::UnitY, box.rotation);
	axes[ 3] = Vector3::Transform(Vector3::UnitZ, box.rotation);
	axes[ 4] = axes[1].Cross(triangle.v1 - triangle.v2);
	axes[ 5] = axes[1].Cross(triangle.v2 - triangle.v3);
	axes[ 6] = axes[1].Cross(triangle.v3 - triangle.v1);
	axes[ 7] = axes[2].Cross(triangle.v1 - triangle.v2);
	axes[ 8] = axes[2].Cross(triangle.v2 - triangle.v3);
	axes[ 9] = axes[2].Cross(triangle.v3 - triangle.v1);
	axes[10] = axes[3].Cross(triangle.v1 - triangle.v2);
	axes[11] = axes[3].Cross(triangle.v2 - triangle.v3);
	axes[12] = axes[3].Cross(triangle.v3 - triangle.v1);

	float distanceMax = -Math::INFTY;
	Vector3 maxAxis{};

	// 軸ごとに判定
	for (auto& axis : axes)
	{
		// 軸の長さが極小ならスキップ
		if (axis.Length() < Math::EPSILON)
		{
			continue;
		}
		// 正規化
		axis.Normalize();

		// 中心間の距離
		float centerDistance = axis.Dot(box.position - triangle.GetCenter());
		if (centerDistance < 0.0f)
		{
			axis *= -1.0f;
			centerDistance *= -1.0f;
		}
		// 三角形の大きさ
		float size1 = Math::Max
		(
			axis.Dot(triangle.v1 - triangle.GetCenter()),
			axis.Dot(triangle.v2 - triangle.GetCenter()),
			axis.Dot(triangle.v3 - triangle.GetCenter())
		);
		// 直方体2の大きさ
		float size2 =
		(
			Math::Abs(axis.Dot(axes[1]) * box.size.x) +
			Math::Abs(axis.Dot(axes[2]) * box.size.y) +
			Math::Abs(axis.Dot(axes[3]) * box.size.z)
		) / 2.0f;

		float distance = centerDistance - size1 - size2;
		if (distance > distanceMax)
		{
			distanceMax = distance;
			maxAxis = axis;
		}
	}

	if (maxAxis.Dot(box.position - triangle.GetCenter()) >= 0.0f)
	{
		maxAxis *= -1.0f;
	}

	*pDirection = maxAxis;
	return distanceMax;
}

// 点から三角形への方向指定最短距離
float Libraries::Math::Geometry::AngledDistance(const Vector3& from, const Triangle& to, const Vector3& direction)
{
	// 三角形を含む平面
	Plane plane{ to.v1, to.v2, to.v3 };

	/*
	 * p = from
	 * v = direction
	 * q = p + tv // 交点
	 * 
	 * ax + by + cz + d = 0 // plane
	 * a = (a, b, c)
	 * a･q = -d
	 * 
	 * -d = a･p + ta･v
	 * t = -(a･p + d) / a･v
	 */

	// レイの倍率
	float t = -(plane.Normal().Dot(from) + plane.D()) / plane.Normal().Dot(direction);

	// 公差位置
	Vector3 intersection = from + direction * t;

	Vector3 normal1 = (to.v1 - to.v2).Cross(to.v1 - intersection);
	Vector3 normal2 = (to.v2 - to.v3).Cross(to.v2 - intersection);
	Vector3 normal3 = (to.v3 - to.v1).Cross(to.v3 - intersection);

	// いずれかの法線方向が逆なら公差位置は三角形の外
	if (Math::Sign(normal1.Dot(normal2)) == -1 || Math::Sign(normal2.Dot(normal3)) == -1)
	{
		return Math::INFTY;
	}
	else
	{
		return t;
	}
}

// 線分同士の方向指定最短距離
float Libraries::Math::Geometry::AngledDistance(const Line& from, const Line& to, const Vector3& direction)
{
	Vector3 a = from.v2 - from.v1;
	Vector3 b = to.v2 - to.v1;
	Vector3 c = direction;
	Vector3 p = from.v1;
	Vector3 q = to.v1;

	/*
	 * p' = p + ta
	 * q' = q + sb
	 * q' - q' = uc
	 * 
	 * q + sb - p - ta = uc
	 * q - p = ta - sb + uc
	 * 
	 * [          ][ t ]
	 * [ a, -b, c ][ s ] = q - p
	 * [          ][ u ]
	 * 
	 * [  t ]   [         ]-1
	 * [ -s ] = [ a, b, c ]  (q - p)
	 * [  u ]   [         ]
	 * 
	 *           1 [ b×c ]
	 *        = ---[ c×a ](q - p)
	 *          det[ a×b ]
	 */

	// 行列式
	float det = a.Dot(b.Cross(c));
	// 行列式が小さければ何もしない
	if (Math::Abs(det) < Math::EPSILON)
	{
		return Math::INFTY;
	}

	float t = b.Cross(c).Dot(q - p) / det;
	float s = a.Cross(c).Dot(q - p) / det;

	if (t < 0.0f || t > 1.0f || s < 0.0f || s > 1.0f)
	{
		return Math::INFTY;
	}

	return a.Cross(b).Dot(q - p) / det;
}

// 点から非回転直方体への方向指定最短距離
float Libraries::Math::Geometry::AngledDistance(const Vector3& from, const NonRotatingBox& to, const Vector3& direction)
{
	// Slab法

	// tの最小値
	float tMin = -Math::INFTY;
	// tの最大値
	float tMax = Math::INFTY;

	// x軸移動量が極小で、直方体の中にないならぶつからない
	if
	(
		Math::Abs(direction.x) < Math::EPSILON &&
		(from.x < to.position.x - to.size.x / 2.0f || from.x > to.position.x + to.size.x / 2.0f)
	)
	{
		return Math::INFTY;
	}
	// y軸移動量が極小で、直方体の中にないならぶつからない
	if
	(
		Math::Abs(direction.y) < Math::EPSILON &&
		(from.y < to.position.y - to.size.y / 2.0f || from.y > to.position.y + to.size.y / 2.0f)
	)
	{
		return Math::INFTY;
	}
	// z軸移動量が極小で、直方体の中にないならぶつからない
	if
	(
		Math::Abs(direction.z) < Math::EPSILON &&
		(from.z < to.position.z - to.size.z / 2.0f || from.z > to.position.z + to.size.z / 2.0f)
	)
	{
		return Math::INFTY;
	}

	float t1 = (to.position.x - to.size.x / 2.0f - from.x) / direction.x;
	float t2 = (to.position.x + to.size.x / 2.0f - from.x) / direction.x;
	if (t1 > t2)
	{
		std::swap(t1, t2);
	}

	tMin = Math::Max(tMin, t1);
	tMax = Math::Min(tMax, t2);
	if (tMin > tMax)
	{
		return Math::INFTY;
	}

	t1 = (to.position.y - to.size.y / 2.0f - from.y) / direction.y;
	t2 = (to.position.y + to.size.y / 2.0f - from.y) / direction.y;
	if (t1 > t2)
	{
		std::swap(t1, t2);
	}

	tMin = Math::Max(tMin, t1);
	tMax = Math::Min(tMax, t2);
	if (tMin > tMax)
	{
		return Math::INFTY;
	}

	t1 = (to.position.z - to.size.z / 2.0f - from.z) / direction.z;
	t2 = (to.position.z + to.size.z / 2.0f - from.z) / direction.z;
	if (t1 > t2)
	{
		std::swap(t1, t2);
	}

	tMin = Math::Max(tMin, t1);
	tMax = Math::Min(tMax, t2);
	if (tMin > tMax)
	{
		return Math::INFTY;
	}

	return tMin;
}

// 点から直方体への方向指定最短距離
float Libraries::Math::Geometry::AngledDistance(const Vector3& from, const Box& to, const Vector3& direction)
{
	// 点と直方体の中心位置の差分
	Vector3 delta = from - to.position;

	// 回転の逆数
	Quaternion rotationInverse;
	to.rotation.Inverse(rotationInverse);

	return AngledDistance
	(
		to.position + Vector3::Transform(delta, rotationInverse),
		NonRotatingBox{ to.position, to.size },
		Vector3::Transform(direction, rotationInverse)
	);
}

// 直方体から三角形への方向指定最短距離
float Libraries::Math::Geometry::AngledDistance(const Box& from, const Triangle& to, const Vector3& direction)
{
	auto boxVertices = from.GetVertices();

	return Math::Min
	(
		AngledDistance(to.v1, from, -direction),
		AngledDistance(to.v2, from, -direction),
		AngledDistance(to.v3, from, -direction),
		AngledDistance(boxVertices.at(0), to, direction),
		AngledDistance(boxVertices.at(1), to, direction),
		AngledDistance(boxVertices.at(2), to, direction),
		AngledDistance(boxVertices.at(3), to, direction),
		AngledDistance(boxVertices.at(4), to, direction),
		AngledDistance(boxVertices.at(5), to, direction),
		AngledDistance(boxVertices.at(6), to, direction),
		AngledDistance(boxVertices.at(7), to, direction),
		AngledDistance(Line{ boxVertices.at(0), boxVertices.at(1) }, Line{ to.v1, to.v2 }, direction),
		AngledDistance(Line{ boxVertices.at(2), boxVertices.at(3) }, Line{ to.v1, to.v2 }, direction),
		AngledDistance(Line{ boxVertices.at(4), boxVertices.at(5) }, Line{ to.v1, to.v2 }, direction),
		AngledDistance(Line{ boxVertices.at(6), boxVertices.at(7) }, Line{ to.v1, to.v2 }, direction),
		AngledDistance(Line{ boxVertices.at(0), boxVertices.at(2) }, Line{ to.v1, to.v2 }, direction),
		AngledDistance(Line{ boxVertices.at(1), boxVertices.at(3) }, Line{ to.v1, to.v2 }, direction),
		AngledDistance(Line{ boxVertices.at(4), boxVertices.at(6) }, Line{ to.v1, to.v2 }, direction),
		AngledDistance(Line{ boxVertices.at(5), boxVertices.at(7) }, Line{ to.v1, to.v2 }, direction),
		AngledDistance(Line{ boxVertices.at(0), boxVertices.at(4) }, Line{ to.v1, to.v2 }, direction),
		AngledDistance(Line{ boxVertices.at(1), boxVertices.at(5) }, Line{ to.v1, to.v2 }, direction),
		AngledDistance(Line{ boxVertices.at(2), boxVertices.at(6) }, Line{ to.v1, to.v2 }, direction),
		AngledDistance(Line{ boxVertices.at(3), boxVertices.at(7) }, Line{ to.v1, to.v2 }, direction),
		AngledDistance(Line{ boxVertices.at(0), boxVertices.at(1) }, Line{ to.v2, to.v3 }, direction),
		AngledDistance(Line{ boxVertices.at(2), boxVertices.at(3) }, Line{ to.v2, to.v3 }, direction),
		AngledDistance(Line{ boxVertices.at(4), boxVertices.at(5) }, Line{ to.v2, to.v3 }, direction),
		AngledDistance(Line{ boxVertices.at(6), boxVertices.at(7) }, Line{ to.v2, to.v3 }, direction),
		AngledDistance(Line{ boxVertices.at(0), boxVertices.at(2) }, Line{ to.v2, to.v3 }, direction),
		AngledDistance(Line{ boxVertices.at(1), boxVertices.at(3) }, Line{ to.v2, to.v3 }, direction),
		AngledDistance(Line{ boxVertices.at(4), boxVertices.at(6) }, Line{ to.v2, to.v3 }, direction),
		AngledDistance(Line{ boxVertices.at(5), boxVertices.at(7) }, Line{ to.v2, to.v3 }, direction),
		AngledDistance(Line{ boxVertices.at(0), boxVertices.at(4) }, Line{ to.v2, to.v3 }, direction),
		AngledDistance(Line{ boxVertices.at(1), boxVertices.at(5) }, Line{ to.v2, to.v3 }, direction),
		AngledDistance(Line{ boxVertices.at(2), boxVertices.at(6) }, Line{ to.v2, to.v3 }, direction),
		AngledDistance(Line{ boxVertices.at(3), boxVertices.at(7) }, Line{ to.v2, to.v3 }, direction),
		AngledDistance(Line{ boxVertices.at(0), boxVertices.at(1) }, Line{ to.v3, to.v1 }, direction),
		AngledDistance(Line{ boxVertices.at(2), boxVertices.at(3) }, Line{ to.v3, to.v1 }, direction),
		AngledDistance(Line{ boxVertices.at(4), boxVertices.at(5) }, Line{ to.v3, to.v1 }, direction),
		AngledDistance(Line{ boxVertices.at(6), boxVertices.at(7) }, Line{ to.v3, to.v1 }, direction),
		AngledDistance(Line{ boxVertices.at(0), boxVertices.at(2) }, Line{ to.v3, to.v1 }, direction),
		AngledDistance(Line{ boxVertices.at(1), boxVertices.at(3) }, Line{ to.v3, to.v1 }, direction),
		AngledDistance(Line{ boxVertices.at(4), boxVertices.at(6) }, Line{ to.v3, to.v1 }, direction),
		AngledDistance(Line{ boxVertices.at(5), boxVertices.at(7) }, Line{ to.v3, to.v1 }, direction),
		AngledDistance(Line{ boxVertices.at(0), boxVertices.at(4) }, Line{ to.v3, to.v1 }, direction),
		AngledDistance(Line{ boxVertices.at(1), boxVertices.at(5) }, Line{ to.v3, to.v1 }, direction),
		AngledDistance(Line{ boxVertices.at(2), boxVertices.at(6) }, Line{ to.v3, to.v1 }, direction),
		AngledDistance(Line{ boxVertices.at(3), boxVertices.at(7) }, Line{ to.v3, to.v1 }, direction)
	);
}
