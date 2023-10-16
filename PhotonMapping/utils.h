#ifndef UTILS_H
#define UTILS_H

#include <cstddef>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <ostream>

namespace math {

	/* Vector */

	struct Vector2 {
		float x, y;

		Vector2();

		Vector2(float x, float y);

	};

	struct Vector3 {
		float x, y, z;

		Vector3();

		Vector3(float x, float y, float z);
	};

	bool operator == (const Vector2& v1, const Vector2& v2);

	bool operator != (const Vector2& v1, const Vector2& v2);

	Vector2 operator + (const Vector2& v1, const Vector2& v2);

	Vector2 operator - (const Vector2& v1, const Vector2& v2);

	Vector2 operator * (const Vector2& v, const float k);

	Vector2 operator * (const float k, const Vector2& v);

	Vector2 operator / (const Vector2& v, const float k);

	std::ostream& operator << (std::ostream& out, const Vector2& v);

	bool operator == (const Vector3& v1, const Vector3& v2);

	bool operator != (const Vector3& v1, const Vector3& v2);

	Vector3 operator + (const Vector3& v1, const Vector3& v2);

	Vector3 operator - (const Vector3& v1, const Vector3& v2);

	Vector3 operator * (const Vector3& v, const float k);

	Vector3 operator * (const float k, const Vector3& v);

	Vector3 operator / (const Vector3& v, const float k);

	std::ostream& operator << (std::ostream& out, const Vector3& v);

	float dotProduct(Vector3 v1, Vector3 v2);

	Vector3 crossProduct(Vector3 v1, Vector3 v2);

	Vector3 normalize(Vector3 v);

	float norm(Vector3 w);

	float getRandomP();

	float getRandomFloat(float lowerBound, float upperBound);

	Vector3 chooseARandomPointFromASphere();

	Vector3 chooseARandomPointFromAHemisphere(Vector3 norm);

	Vector3 chooseAPoinCosineDistribution(Vector3 norm);

}

/* 32-bit Color */
struct Color {
	uint8_t r, g, b, a;

	float fr() { return r * 1.0f / 255.0f; }
	float fg() { return g * 1.0f / 255.0f; }
	float fb() { return b * 1.0f / 255.0f; }
	float fa() { return a * 1.0f / 255.0f; }

	uint32_t get_rgba() { return r << 24 | g << 16 | b << 8 | a; }
	uint32_t get_argb() { return a << 24 | r << 16 | g << 8 | b; }

};

struct Ray {
	math::Vector3 origin;
	math::Vector3 direction;
};

struct Hit {
	math::Vector3 normal;
	math::Vector2 uv;
	struct Material {
		float metallic;
		float roughness;
		bool double_sided;
	} material;
};

namespace geometry {
	using namespace math;
	struct Mesh
	{
		std::vector<Vector3> positions;
		std::vector<Vector3> normals;
		std::vector<Vector2> texcoords;

		std::vector<unsigned int> indices;

	};
}
#endif