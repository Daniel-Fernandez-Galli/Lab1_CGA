#include "utils.h"

/* Vector */

math::Vector::Vector(std::initializer_list<float> values) : data{ values } { }

float math::norm2(const Vector& v) {
	float ret = 0;
	for (const float& x : v.data) {
		ret += x * x;
	}
	return std::sqrt(ret);
}

bool math::operator ==(const Vector& v1, const Vector& v2) {
	return v1.data == v2.data;
}

bool math::operator !=(const Vector& v1, const Vector& v2) {
	return v1.data != v2.data;
}

math::Vector math::operator +(const Vector& v1, const Vector& v2) {
	if (v1.data.size() != v2.data.size()) {
		throw std::domain_error("Exception: Vectors must be same size.");
	}

	Vector ret = v1;
	for (int i = 0; i < ret.data.size(); i++) {
		ret.data[i] += v2.data[i];
	}
	return ret;
}

math::Vector math::operator -(const Vector& v1, const Vector& v2) {
	if (v1.data.size() != v2.data.size()) {
		throw std::domain_error("Exception: Vectors must be same size.");
	}

	Vector ret = v1;
	for (int i = 0; i < ret.data.size(); i++) {
		ret.data[i] -= v2.data[i];
	}
	return ret;
}

math::Vector math::operator -(const Vector& v1)
{
	return (-1) * v1;
}

math::Vector math::operator *(const Vector& v, const float k) {
	Vector ret = v;
	for (int i = 0; i < ret.data.size(); i++) {
		ret.data[i] *= k;
	}
	return ret;
}

math::Vector math::operator *(const float k, const Vector& v) {
	return v * k;
}

math::Vector math::operator /(const Vector& v, const float k) {
	return v * (1 / k);
}

math::Vector math::operator+=(Vector& v1, const Vector& v2)
{
	return v1 = v1 + v2;
}

math::Vector math::operator-=(Vector& v1, const Vector& v2)
{
	return v1 = v1 - v2;
}

math::Vector math::operator*=(Vector& v, const float k)
{
	return v = v*k;
}

std::ostream& math::operator <<(std::ostream& out, const Vector& v) {
	out << "(";
	for (int i = 0; i < v.data.size() - 1; i++) {
		out << v.data[i] << ", ";
	}
	out << v.data.back() << ")";
	return out;
}

float math::dot_product(const Vector& v1, const Vector& v2) {
	if (v1.data.size() != v2.data.size()) {
		throw std::domain_error("Exception: Vectors must be same size.");
	}

	float ret = 0;
	for (int i = 0; i < v1.data.size(); i++) {
		ret += v1.data[i] * v2.data[i];
	}
	return std::sqrt(ret);
}

math::Vector math::cross_product(const Vector& v1, const Vector& v2) {
	if (v1.data.size() != 3 ||
		v2.data.size() != 3) {
		throw std::domain_error("Exception: Vectors must be three dimensional.");
	}

	Vector ret{ 0,0,0 };

	ret.data[1] = v1.data[2] * v2.data[3] - v1.data[3] * v2.data[2];
	ret.data[2] = v1.data[3] * v2.data[1] - v1.data[1] * v2.data[3];
	ret.data[3] = v1.data[1] * v2.data[2] - v1.data[2] * v2.data[1];

	ret = ret / norm2(ret);

	return ret;
}

math::Vector3::Vector3(float x, float y, float z) : Vector{x,y,z}, x(x), y(y), z(z){}

math::Vector2::Vector2(float x, float y) : Vector{x,y}, x(x), y(y) {}
