#include "matrix.h"

#include <sstream>

using galois::matrix;
using galois::symbol;

static constexpr size_t n = 4;
static constexpr size_t k = 3;

template<size_t n, size_t k>
static matrix<n, k> vandermonde()
{
	matrix<n, k> result;
	for (uint8_t r = 0; r < matrix<n, k>::n_rows; ++r)
	{
		for (uint8_t c = 0; c < matrix<n, k>::n_cols; ++c)
		{
			result(r, c) = pow(r, c);
		}
	}
	return result;
}
template<size_t n, size_t k>
static matrix<n, k> build_matrix()
{
	auto vm = vandermonde<n, k>();
	auto inv = vm.submatrix<k, k>().inverse();
	return vm * inv;
}

symbol mat[] = {
	0, 1, 1,
	1, 0, 1,
	1, 1, 1
};

int main()
{
	auto m = matrix<3, 3>(mat);
	auto n = m.inverse();
	auto x = m * n;
}
