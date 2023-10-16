#ifndef UTILS_H
#define UTILS_H

#include <cstddef>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <ostream>
#include <array>

enum class Direction {
	FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN
};

namespace math {

	constexpr float pi = 3.14159267f;

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

	float dot_product(const Vector3& v1, const Vector3& v2);

	Vector3 cross_product(const Vector3& v1, const Vector3& v2);

	Vector3 normalize(const Vector3& v);

	float norm2(const Vector3& v);

	/* Given vertex shading normals n1, n2, n3, returns shading normal at triangle barycentric coordinates u,v  */
	Vector3 normal_interpolation(const Vector3& n1, const Vector3& n2, const Vector3& n3, float u, float v);

	/* MATRIX */
	template <size_t rows, size_t columns>
	using Matrix = std::array<std::array<float, columns>, rows>;

	template <size_t n>
	Matrix<n, n> identity();

	template <size_t n, size_t m>
	Matrix<n, m> operator + (const Matrix<n, m>& matrix_1, const Matrix<n, m>& matrix_2);

	template <size_t n, size_t m>
	Matrix<n, m> operator - (const Matrix<n, m>& matrix_1, const Matrix<n, m>& matrix_2);

	template <size_t n, size_t m>
	Matrix<n, m> operator - (const Matrix<n, m>& matrix);

	template <size_t n, size_t m>
	Matrix<n, m> operator * (const Matrix<n, m>& matrix, const float k);

	template <size_t n, size_t m>
	Matrix<n, m> operator * (const float k, const Matrix<n, m>& matrix);

	template <size_t l, size_t n, size_t m>
	Matrix<l, n> operator * (const Matrix<l, m>& matrix_1, const Matrix<m, n>& matrix_2);

	template <size_t n, size_t m>
	Matrix<n, m> operator / (const Matrix<n, m>& matrix, const float k);

	template <size_t n, size_t m>
	std::ostream& operator << (std::ostream& out, const Matrix<n, m>& matrix);

	/* Quaternions must be defined in xyzw format */
	Matrix<4, 4> quaternion_to_rotation_matrix(const Matrix<4, 1>& quaternion);

	template <size_t n, size_t m>
	void swap_rows(Matrix<n, m>& matrix, int i, int j);

	template <size_t n, size_t m>
	void scale_row(Matrix<n, m>& matrix, int i, double scalar);

	template <size_t n, size_t m>
	void add_rows(Matrix<n, m>& matrix, int i, int j, double scalar);

	template <size_t n>
	bool invert_matrix(Matrix<n, n>& matrix);

	/* END MATRIX */


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

struct Material {
	float metallic;
	float roughness;
	bool double_sided;
};

struct Hit {
	math::Vector3 normal;
	math::Vector2 uv;
	Material material{};
};

namespace geometry {
	using namespace math;
	struct Mesh
	{
		std::vector<Vector3> positions;
		std::vector<Vector3> normals;
		std::vector<Vector2> texcoords;

		std::vector<unsigned int> indices;

		Mesh() = default;
		Mesh(const Mesh&) = delete;
		Mesh operator=(const Mesh&) = delete;

	};

}

/* Template definitions */
#ifndef MATRIX_TPP
#define MATRIX_TPP
#include "Matrix.tpp"
#endif

#endif