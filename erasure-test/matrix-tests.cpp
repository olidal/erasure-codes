#include "matrix.h"
#include <iostream>

using erasure::symbol;
using erasure::matrix;
using erasure::inverse;

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

	matrix m2 = m;
	if (!inverse(m))
		return false;

	return memcmp(m.data(), m2.data(), 
		sizeof(symbol) * m.size()) == 0;
}

bool test_inverse(size_t sz)
{
	matrix m = erasure::vandermonde(sz, sz);
	matrix m2 = m;
	
	// A square vandermonde matrix is always invertible
	if (!inverse(m2))
		return false;

	return m * m2 == matrix(sz, sz, 1);
}

int main()
{
	bool result = true;

	for (size_t i = 1; i < 128; ++i)
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
