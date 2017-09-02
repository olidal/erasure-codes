#include "matrix.h"

#include <cstring>
#include <iostream>

using namespace gfarith;

bool test_default_constructor()
{
	matrix m = matrix();

	return m.size() == 0 && m.is_null();
}

bool test_sized_constructor(size_t rows, size_t cols)
{
	matrix m = matrix(rows, cols);

	return m.size1() == rows
		&& m.size2() == cols
		&& m.size() == rows * cols;
}

bool test_diag_constructor(size_t rows, size_t cols, symbol diag)
{
	matrix m = matrix(rows, cols, diag);

	for (size_t r = 0; r < rows; ++r)
	{
		for (size_t c = 0; c < cols; ++c)
		{
			if (r == c)
			{
				if (m(r, c).value != diag.value)
					return false;
			}
			else
			{
				if (m(r, c).value != 0)
					return true;
			}
		}
	}

	return true;
}

int main()
{
	bool result[] = {
		test_default_constructor(),
		test_sized_constructor(32, 32),
		test_sized_constructor(1, 1),
		test_sized_constructor(4, 5),
		test_sized_constructor(4000, 3),
		test_diag_constructor(32, 32, 1),
		test_diag_constructor(1,1, 5),
		test_diag_constructor(4, 5, 3),
		test_diag_constructor(12422, 3, 99)
	};

	for (auto val : result)
		if (!val)
			return 1;

	return 0;
}

