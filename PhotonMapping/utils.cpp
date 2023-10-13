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

float math::dotProduct(Vector3 v1, Vector3 v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Vector3 math::crossProduct(Vector3 v1, Vector3 v2)
{
	Vector3 result;

	result.x = v1.y * v2.z - v1.z * v2.y;
	result.y = v1.z * v2.x - v1.x * v2.z;
	result.z = v1.x * v2.y - v1.y * v2.x;

	result = normalize(result);

	return result;
}

Vector3 math::normalize(Vector3 w) {
	Vector3 v = w;
	// Calculate the magnitude of the cross product
	float magnitude = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);

	// Normalize the cross product
	v.x /= magnitude;
	v.y /= magnitude;
	v.z /= magnitude;

	return v;
}

float math::norm(Vector3 w) {
	Vector3 v = w;
	float magnitude = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	return magnitude;
}