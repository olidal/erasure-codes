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
	constexpr size_t sz = 128;

	matrix<sz, sz>* matrices = new matrix<sz, sz>[3];

	matrix<sz, sz>& mat = matrices[0];
	matrix<sz, sz>& inv = matrices[1];
	matrix<sz, sz>& identity = matrices[2];

	mat = vandermonde<sz, sz>();
	inv = mat.inverse();
	identity = matrix<sz, sz>(1);

	REQUIRE(inv * mat == identity);

	delete[] matrices;
}
