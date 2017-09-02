#include <matrix.h>

#include <cstring>

#include "catch-wrapper.hpp"

using namespace gfarith;

constexpr size_t sizes[] = { 1, 3, 4, 5, 9, 129 };

TEST_CASE("row-assign-test", "[matrix]")
{
	for (size_t sz : sizes)
	{
		matrix m1(sz, 5);
		matrix m2(sz, 5);

		std::memset(m1.data(), 0x45, m1.size() * sizeof(symbol));
		std::memset(m2.data(), 0x22, m2.size() * sizeof(symbol));

		for (size_t i = 0; i < m1.size1(); ++i)
		{
			m1[i] = m2[i];
		}

		REQUIRE(m1 == m2);
	}
}
