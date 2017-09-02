#include <symbol.h>

#include <iostream>

typedef uint8_t(*arith_proc)(uint8_t, uint8_t);

bool is_commutative(arith_proc proc)
{
	for (size_t i = 0; i < 256; ++i)
	{
		for (size_t j = 0; j < 256; ++j)
		{
			uint8_t r1 = proc((uint8_t)i, (uint8_t)j);
			uint8_t r2 = proc((uint8_t)i, (uint8_t)j);

			if (r1 != r2)
				return false;
		}
	}

	return true;
}

bool mul_by_zero()
{
	for (size_t i = 0; i < 256; ++i)
	{
		if (gfarith::mul(0, (uint8_t)i) != 0)
			return false;
	}

	return true;
}
bool add_test()
{
	for (size_t i = 0; i < 256; ++i)
	{
		if (gfarith::add(0, (uint8_t)i) != i)
			return false;
	}

	return true;
}

int main()
{
	if (!is_commutative(&gfarith::add))
	{
		std::cout << "Error: add is not commutative!" << std::endl;
		return 1;
	}

	// Subtraction is commutative in GF(2^8)
	if (!is_commutative(&gfarith::sub))
	{
		std::cout << "Error: sub is not commutative!" << std::endl;
		return 1;
	}

	if (!is_commutative(&gfarith::mul))
	{
		std::cout << "Error: mul is not commutative!" << std::endl;
		return 1;
	}

	if (!mul_by_zero())
	{
		std::cout << "Error: Multiplying by 0 does not yield 0!" << std::endl;
		return 1;
	}

	if (!add_test())
	{
		std::cout << "Error: 0 + x != x" << std::endl;
		return 1;
	}

	return 0;
}
