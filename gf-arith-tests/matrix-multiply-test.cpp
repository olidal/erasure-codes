#include "matrix.h"

#include <cstring>
#include <iostream>

using namespace gfarith;

bool operator ==(const matrix& a, const matrix& b)
{
	if (a.size1() != b.size1() || a.size2() != b.size2())
		return false;

	return memcmp(a.data(), b.data(), a.size() * sizeof(symbol)) == 0;
}

bool test_multiplication(size_t sz)
{
	matrix m = matrix(sz, sz, 1);

	// Identity multiplied by itself is identity
	return m * m == m;
}

int main()
{
	bool result = true;

	for (size_t i = 1; i < 128; ++i)
	{
		if (!test_multiplication(i))
		{
			std::cout << "Matrix multiplication failed with dimensions ("
				<< i << ", " << i << ")" << std::endl;
			result = false;
		}
	}

	return result ? 0 : 1;
}
