#include "utils.h"
template <size_t n>
math::Matrix<n, n> math::identity()
{
	Matrix<n, n> id = { };
	for (int i = 0; i < n; i++) {
		id[i][i] = 1;
	}
	return id;
}

template<size_t n, size_t m>
math::Matrix<n, m> math::operator+(const Matrix<n, m>& matrix_1, const Matrix<n, m>& matrix_2)
{
	Matrix<n, m> sum = { };
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			sum[i][j] = matrix_1[i][j] + matrix_2[i][j];
		}
	}
	return sum;
}

template<size_t n, size_t m>
math::Matrix<n, m> math::operator-(const Matrix<n, m>& matrix_1, const Matrix<n, m>& matrix_2)
{
	return matrix_1 + (-matrix_2);
}

template<size_t n, size_t m>
math::Matrix<n, m> math::operator-(const Matrix<n, m>& matrix)
{
	Matrix<n, m> neg = { };
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			neg[i][j] = -matrix[i][j];
		}
	}
	return neg;
}

template<size_t n, size_t m>
math::Matrix<n, m> math::operator*(const Matrix<n, m>& matrix, const float k)
{
	Matrix<n, m> prod = { };
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			prod[i][j] = matrix[i][j] * k;
		}
	}
	return prod;
}

template<size_t n, size_t m>
math::Matrix<n, m> math::operator*(const float k, const Matrix<n, m>& matrix)
{
	return matrix * k;
}

template <size_t l, size_t n, size_t m>
math::Matrix<l, n> math::operator*(const Matrix<l, m>& matrix_1, const Matrix<m, n>& matrix_2)
{
	Matrix<l, n> prod = { };
	for (int i = 0; i < l; i++) {
		for (int j = 0; j < n; j++) {
			for (int k = 0; k < m; k++) {
				prod[i][j] += matrix_1[i][k] * matrix_2[k][j];
			}
		}
	}
	return prod;
}

template<size_t n, size_t m>
math::Matrix<n, m> math::operator/(const Matrix<n, m>& matrix, const float k)
{
	return matrix * (1 / k);
}

template<size_t n, size_t m>
std::ostream& math::operator<<(std::ostream& os, const Matrix<n, m>& matrix)
{
	for (int i = 0; i < n; i++) {
		os << "|  ";
		for (int j = 0; j < m; j++) {
			os << matrix[i][j] << "  ";
		}
		os << "|\n";
	}
	return os;
}

template<size_t n, size_t m>
void math::swap_rows(Matrix<n, m>& matrix, int i, int j)
{
	for (int k = 0; k < m; k++) {
		double temp = matrix[i][k];
		matrix[i][k] = matrix[j][k];
		matrix[j][k] = temp;
	}
}

template<size_t n, size_t m>
void math::scale_row(Matrix<n, m>& matrix, int i, double scalar)
{
	for (int k = 0; k < m; k++) {
		matrix[i][k] *= scalar;
	}
}

template<size_t n, size_t m>
void math::add_rows(Matrix<n, m>& matrix, int i, int j, double scalar)
{
	for (int k = 0; k < m; k++) {
		matrix[i][k] += scalar * matrix[j][k];
	}
}

template<size_t n>
bool math::invert_matrix(Matrix<n, n>& matrix)
{
	// Create the augmented matrix
	Matrix<4, 8>augmented{};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			augmented[i][j] = matrix[i][j];
			augmented[i][j + 4] = (i == j) ? 1.0f : 0.0f;
		}
	}

	// Perform Gauss-Jordan elimination
	for (int i = 0; i < 4; i++) {
		// Find the pivot row and scale it
		int pivot_row = -1;
		double max_val = 0.0;
		for (int j = i; j < 4; j++) {
			if (abs(augmented[j][i]) > max_val) {
				max_val = abs(augmented[j][i]);
				pivot_row = j;
			}
		}
		if (pivot_row == -1) {
			// Matrix is singular
			return false;
		}
		swap_rows(augmented, i, pivot_row);
		scale_row(augmented, i, 1.0 / augmented[i][i]);
		for (int j = 0; j < 4; j++) {
			if (j != i) {
				add_rows(augmented, j, i, -augmented[j][i]);
			}
		}
	}

	// Extract the right side of the augmented matrix, which is the inverse
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			matrix[i][j] = augmented[i][j + 4];
		}
	}

	return true;
}
