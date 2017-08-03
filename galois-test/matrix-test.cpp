#include "matrix.h"

#include <gtest\gtest.h>

using namespace galois;

TEST(matrix, equal_to_self)
{
	symbol matrix_data[] = {
		0, 1, 1,
		1, 0, 1,
		1, 1, 0
	};

	matrix<3, 3> mat = matrix_data;

	ASSERT_EQ(mat, mat);
}
TEST(matrix, inverse_multiplied)
{
	symbol matrix_data[] = {
		0, 1, 1,
		1, 0, 1,
		1, 1, 0
	};

	matrix<3, 3> mat = matrix_data;
	matrix<3, 3> inv = mat.inverse();
	matrix<3, 3> identity = matrix<3, 3>(1);
	ASSERT_EQ(inv * mat, identity);
}
