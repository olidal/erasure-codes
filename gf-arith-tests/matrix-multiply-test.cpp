#include "matrix.h"

#include <cstring>
#include <iostream>

#include "catch-wrapper.hpp"

using namespace gfarith;

TEST_CASE("indentity multiplication", "[matrix]")
{
	constexpr size_t sizes[] = { 1, 3, 4, 5, 9, 129 };
	for (size_t sz : sizes)
	{
		matrix m = matrix(sz, sz, 1);

		REQUIRE(m * m == m);
	}
}
