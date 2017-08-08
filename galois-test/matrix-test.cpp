#include "matrix.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

using namespace galois;

symbol matrix_data[] = {
	1, 0, 0,
	1, 1, 1,
	1, 2, 4
};

TEST_CASE("Matrix equal to self", "[matrix]")
{
	matrix<3, 3> mat = matrix_data;

	REQUIRE(mat == mat);
}
TEST_CASE("Matrix inverse * self is identity", "[matrix]")
{
	matrix<3, 3> mat = matrix_data;
	matrix<3, 3> inv = mat.inverse();
	auto result = inv * mat;
	matrix<3, 3> identity = matrix<3, 3>(1);
	REQUIRE(inv * mat == identity);
}

TEST_CASE("Large inverse", "[matrix]")
{
	
}
