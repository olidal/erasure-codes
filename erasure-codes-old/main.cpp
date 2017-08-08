#include "galois.h"
#include "interface.h"
#include "matrix.h"

#include <sstream>

using namespace erasure;

typedef galois_field<uint8_t> field;

static matrix vandermonde(size_t rows, size_t cols)
{
	matrix result = matrix(rows, cols);
	for (uint8_t r = 0; r < rows; ++r)
	{
		for (uint8_t c = 0; c < cols; ++c)
		{
			result(r, c) = symbol_t::exp(r, c);
		}
	}
	return result;
}
static matrix build_matrix(size_t n, size_t k)
{
	matrix vm = vandermonde(n, k);

	return vm * inverse(vm.submatrix(k, k));
}

void print_mat(matrix m)
{
	std::stringstream ss;
	ss << "{";
	bool a = false, b;

	for (size_t r = 0; r < m.n_rows; ++r)
	{
		if (a)
			ss << ',';

		b = false;
		a = true;

		ss << '{';

		for (size_t c = 0; c < m.n_cols; ++c)
		{
			if (b)
				ss << ',';
			b = true;

			ss << (size_t)m(r, c).value;
		}

		ss << '}';
	}
	ss << '}';

	printf("%s\n", ss.str().c_str());
}

matrix mat1()
{
	matrix m = matrix(4, 4);

	size_t i = 0;
	for (size_t r = 0; r < 4; ++r)
	{
		for (size_t c = 0; c < 4; ++c)
		{
			m(r, c) = field::exp(++i, ++i);
		}
	}

	return m;
}



inline matrix inverse2(matrix m)
{
	assert(m.n_rows == m.n_cols);
	m.append(matrix(m.n_rows, m.n_cols, 1));

	for (size_t r = 0; r < m.n_rows; ++r)
	{
		// If the diagonal is 0 then swap with the 
		// first row below that doesn't have a 0
		if (m(r, r) == 0)
		{
			for (size_t i = r + 1; i < m.n_rows; ++i)
			{
				if (m(i, r) != 0)
				{
					m.swaprows(r, i);
				}
			}

		}
		
		// If the diagonal is still 0 then
		// the matrix is singular
		assert(m(r, r) != 0);

		{
			// Scale the row so that the
			// diagonal element is 1
			symbol_t div = m(r, r);

			for (size_t c = 0; c < m.n_cols; ++c)
			{
				m(r, c) /= div;
			}
		}

		printf("o ");
		print_mat(m);

		for (size_t i = r + 1; i < m.n_rows; ++i)
		{
			symbol_t scale = m(i, r);
			for (size_t c = 0; c < m.n_cols; ++c)
			{
				m(i, c) -= scale * m(r, c);
			}
		}

		printf("o ");
		print_mat(m);
	}

	printf("o ");
	print_mat(m);

	for (size_t d = 0; d < m.n_rows; ++d)
	{
		for (size_t i = 0; i < d; ++i)
		{
			symbol_t scale = m(i, d);
			for (size_t c = 0; c < m.n_cols; ++c)
			{
				m(i, c) -= scale * m(d, c);
			}
		}
	}

	printf("o ");
	print_mat(m);

	return m.submatrix(0, m.n_rows, m.n_rows, m.n_cols);
}

int main()
{
	field::init();

	matrix m = matrix(4, 4, 3);
	print_mat(m);
	matrix i = inverse2(m);
	print_mat(i);
	print_mat(m * i);

	printf("%d", (symbol_t(1) / 2).value);

	//print_mat(build_matrix(n, k));
}
