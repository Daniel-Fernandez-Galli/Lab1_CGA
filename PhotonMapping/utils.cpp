#include "utils.h"

/* Vector */

using namespace math;

Vector2::Vector2() : x(0), y(0) {}

Vector2::Vector2(float x, float y) : x(x), y(y) {}

Vector3::Vector3() : x(0), y(0), z(0) {}

Vector3::Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

bool math::operator == (const Vector2& v1, const Vector2& v2) {
	return (v1.x == v2.x) && (v1.y == v2.y);
}

bool math::operator != (const Vector2& v1, const Vector2& v2) {
	return !(v1 == v2);
}

Vector2 math::operator + (const Vector2& v1, const Vector2& v2) {
	return Vector2(v1.x + v2.x, v1.y + v2.y);
}

Vector2 math::operator - (const Vector2& v1, const Vector2& v2) {
	return Vector2(v1.x - v2.x, v1.y - v2.y);
}

Vector2 math::operator * (const Vector2& v, const float k) {
	return Vector2(v.x * k, v.y * k);
}

Vector2 math::operator*(const float k, const Vector2& v)
{
	return Vector2(k * v.x, k * v.y);
}

Vector2 math::operator / (const Vector2& v, const float k) {
	return Vector2(v.x / k, v.y / k);
}

std::ostream& math::operator << (std::ostream& out, const Vector2& v) {
	return out << "(" << v.x << ", " << v.y << ")";
}

bool math::operator == (const Vector3& v1, const Vector3& v2) {
	return (v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z);
}

bool math::operator != (const Vector3& v1, const Vector3& v2) {
	return !(v1 == v2);
}

Vector3 math::operator + (const Vector3& v1, const Vector3& v2) {
	return Vector3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

Vector3 math::operator - (const Vector3& v1, const Vector3& v2) {
	return Vector3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

Vector3 math::operator * (const Vector3& v, const float k) {
	return Vector3(v.x * k, v.y * k, v.z * k);
}

Vector3 math::operator*(const float k, const Vector3& v)
{
	return Vector3(k * v.x, k * v.y, k * v.z);
}

Vector3 math::operator / (const Vector3& v, const float k) {
	return Vector3(v.x / k, v.y / k, v.z / k);
}

std::ostream& math::operator<<(std::ostream& out, const Vector3& v)
{
	return out << "(" << v.x << ", " << v.y << v.z << ")";
}

float math::dot_product(const Vector3 &v1, const Vector3 &v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Vector3 math::cross_product(const Vector3 &v1, const Vector3 &v2)
{
	Vector3 result;

	result.x = v1.y * v2.z - v1.z * v2.y;
	result.y = v1.z * v2.x - v1.x * v2.z;
	result.z = v1.x * v2.y - v1.y * v2.x;

	result = normalize(result);

	return result;
}

Vector3 math::normalize(const Vector3 &v) {
	Vector3 n = v;
	float magnitude = norm2(v);
	n.x /= magnitude;
	n.y /= magnitude;
	n.z /= magnitude;
	return n;
}

float math::norm2(const Vector3 &v) {
	Vector3 n = v;
	float magnitude = std::sqrt(n.x * n.x + n.y * n.y + n.z * n.z);
	return magnitude;
}

Vector3 math::normal_interpolation(const Vector3& n1, const Vector3& n2, const Vector3& n3, float u, float v)
{
	return (1.0f - u - v) * n1 + u * n2 + v * n3;
}

Matrix<4, 4> math::quaternion_to_rotation_matrix(const Matrix<4, 1>& quaternion)
{
	float x = quaternion[0][0];
	float y = quaternion[1][0];
	float z = quaternion[2][0];
	float w = quaternion[3][0];
	return {
		std::array<float,4>	{	1 - 2 * (y * y - z * z),			  2 * (x * y - z * w),			  2 * (x * z + y * w),			0	},
		std::array<float,4>	{	  2 * (x * y + w * z),			1 - 2 * (x * x - z * z),			  2 * (y * z - w * x),			0	},
		std::array<float,4>	{	  2 * (x * z - w * y),			  2 * (y * z + w * x),			1 - 2 * (x * x - y * y),			0	},
		std::array<float,4>	{		0,								0,							0,					1	}
	};
}