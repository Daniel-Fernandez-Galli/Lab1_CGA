#ifndef UTILS_H
#define UTILS_H

#include <cstddef>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <ostream>

namespace math {

/* Vector */

	/* Generic n-dimensional vector
	 Must be initialized with brackets
	 Example: 
	 Vector myVec{1,2,3};
	 or
	 Vector myVec({1,2,3});
	*/
	struct Vector {
		
		std::vector<float> data;

		Vector(std::initializer_list<float> values);
	};

	/* Returns the euclidean norm of v */
	float norm2(const Vector& v);

	bool operator ==(const Vector& v1, const Vector& v2);

	bool operator !=(const Vector& v1, const Vector& v2);

	Vector operator +(const Vector& v1, const Vector& v2);
	
	Vector operator -(const Vector& v1, const Vector& v2);

	Vector operator -(const Vector& v1);

	Vector operator *(const Vector& v, const float k);

	Vector operator *(const float k, const Vector& v);

	Vector operator /(const Vector& v, const float k);

	Vector operator +=(Vector& v1, const Vector& v2);

	Vector operator -=(Vector& v1, const Vector& v2);

	Vector operator *=(Vector& v, const float k);

	std::ostream& operator <<(std::ostream& out, const Vector& v);

	float dot_product(const Vector& v1, const Vector& v2);

	/* Only supports 3D Vectors */
	Vector cross_product(const Vector& v1, const Vector& v2);
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

#endif
