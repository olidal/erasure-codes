#include <symbol.h>

#include <iostream>

#include "catch-wrapper.hpp"

typedef uint8_t(*arith_proc)(uint8_t, uint8_t);

uint8_t gf_pow(uint8_t a, uint8_t b)
{
	uint8_t prod = 1;

	while (b > 1)
	{
		if (b & 1)
		{
			prod = gfarith::mul(prod, a);
			b -= 1;
		}

		prod = gfarith::mul(prod, prod);
		b /= 2;
	}

	if (b == 1)
		prod = gfarith::mul(prod, a);

	return prod;
}

bool is_commutative(arith_proc proc, size_t min = 0)
{
	for (size_t i = min; i < 256; ++i)
	{
		for (size_t j = min; j < 256; ++j)
		{
			uint8_t r1 = proc((uint8_t)i, (uint8_t)j);
			uint8_t r2 = proc((uint8_t)j, (uint8_t)i);

			if (r1 != r2)
				return false;
		}
	}

	return true;
}

TEST_CASE("multiplication is commutative", "[symbol]")
{
	REQUIRE(is_commutative(&gfarith::mul));
}
TEST_CASE("addition is commutative", "[symbol]")
{
	REQUIRE(is_commutative(&gfarith::add));
}
TEST_CASE("subtraction is commutative", "[symbol]")
{
	REQUIRE(is_commutative(&gfarith::sub));
}
TEST_CASE("division is not commutative", "[symbol]")
{
	REQUIRE(!is_commutative(&gfarith::div, 1));
}
TEST_CASE("exponentiation is repeated multiplication", "[symbol]")
{
	static constexpr uint8_t exponents[] = { 0, 1, };// 5, 44, 192 };
	static constexpr uint8_t bases[] = { 0, 1, };// 55, 12, 183 };

	for (uint8_t exp : exponents)
	{
		for (uint8_t base : bases)
		{
			uint8_t r1 = gf_pow(base, exp);
			uint8_t r2 = gfarith::exp(base, exp);

			REQUIRE(r1 == r2);
		}
	}
}

TEST_CASE("multiplication by 0 is 0", "[symbol]")
{
	for (size_t i = 0; i < 256; ++i)
	{
		REQUIRE(gfarith::mul(0, (uint8_t)i) == 0);
	}
}
TEST_CASE("0 + x = x", "[symbol]")
{
	for (size_t i = 0; i < 256; ++i)
	{
		REQUIRE(gfarith::add(0, (uint8_t)i) == i);
	}
}

TEST_CASE("2^2 == 0x04", "[symbol]")
{
	uint8_t a = gfarith::exp(2, 2);

	REQUIRE(a == 0x4);
}
