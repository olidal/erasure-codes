#include "matrix.h"
#include <iostream>

using gfarith::symbol;
using gfarith::matrix;

matrix vandermonde(size_t n, size_t k)
{
	matrix m{ n, k };

	for (size_t r = 0; r < n; ++r)
	{
		for (size_t c = 0; c < k; ++c)
		{
			m(r, c) = gfarith::exp((uint8_t)r, (uint8_t)c);
		}
	}

	return m;
}

bool operator ==(const matrix& a, const matrix& b)
{
	if (a.size1() != b.size1() || a.size2() != b.size2())
		return false;

	return memcmp(a.data(), b.data(), a.size() * sizeof(symbol)) == 0;
}

bool test_identity_inverse(size_t sz)
{
	matrix m{ sz, sz };
	memset(m.data(), 0, sizeof(symbol) * m.size());

	for (size_t i = 0; i < sz; ++i)
		m(i, i) = 1;

	matrix m2 = m.inverse();

	return memcmp(m.data(), m2.data(), 
		sizeof(symbol) * m.size()) == 0;
}

bool test_inverse(size_t sz)
{
	matrix m = vandermonde(sz, sz);
	matrix m2 = m.inverse();
	matrix r = m * m2;

	return r == matrix(sz, sz, 1);
}

int main()
{
	bool result = true;

	for (size_t i = 3; i < 64; ++i)
	{
		if (!test_identity_inverse(i))
		{
			std::cout << "Identity inverse failed with dimensions (" 
				<< i << ", " << i << ")" << std::endl;
			result = false;
		}

		if (!test_inverse(i))
		{
			std::cout << "Vandermonde inverse failed with dimensions ("
				<< i << ", " << i << ")" << std::endl;
			result = false;
		}
	}

	return result ? 0 : 1;
}
