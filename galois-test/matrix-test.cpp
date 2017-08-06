#include "matrix.h"

#include "catch.hpp"

using namespace galois;

TEST_CASE("Matrix equal to self", "[matrix]")
{
	symbol matrix_data[] = {
		0, 1, 1,
		1, 0, 1,
		1, 1, 0
	};

	matrix<3, 3> mat = matrix_data;

	REQUIRE(mat == mat);
}
TEST_CASE("Matrix inverse * self is identity", "[matrix]")
{
	symbol matrix_data[] = {
		0, 1, 1,
		1, 0, 1,
		1, 1, 0
	};

	matrix<3, 3> mat = matrix_data;
	matrix<3, 3> inv = mat.inverse();
	auto result = inv * mat;
	matrix<3, 3> identity = matrix<3, 3>(1);
	REQUIRE(inv * mat == identity);
}
