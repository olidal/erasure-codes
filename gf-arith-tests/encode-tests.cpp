#include <encode.h>
#include <matrix.h>

#include <immintrin.h>
#include <cstdlib>
#include <cstring>
#include <numeric>

using namespace gfarith;

matrix vandermonde(size_t n, size_t k)
{
	matrix m{ n, k };

	for (size_t r = 0; r < n; ++r)
	{
		for (size_t c = 0; c < k; ++c)
		{
			m(r, c) = gfarith::exp((uint8_t)r, (uint8_t)c);
		}
	}

	return m;
}

typedef decltype(&adv::mul_row) mul_row_proc;
typedef mul_row_proc mul_add_row_proc;

bool test_method(
	size_t num_bytes, 
	mul_row_proc mul_add,
	uint8_t expected)
{
	uint8_t* in = (uint8_t*)_mm_malloc(sizeof(uint8_t) * num_bytes, 64);
	uint8_t* out = (uint8_t*)_mm_malloc(sizeof(uint8_t) * num_bytes, 64);

	std::memset(in, 0x8, sizeof(uint8_t) * num_bytes);
	std::memset(out, 0x63, sizeof(uint8_t) * num_bytes);

	mul_add(0x31, in, out, num_bytes);
	
	bool result = true;

	for (size_t i = 0; i < num_bytes; ++i)
	{
		if (out[i] != expected)
		{
			result = false;
			break;
		}
	}

	_mm_free(in);
	_mm_free(out);

	return result;
}

bool mul_add_row_test(
	size_t num_bytes,
	mul_add_row_proc mul_add)
{
	return test_method(
		num_bytes,
		mul_add,
		gfarith::add(0x63, gfarith::mul(0x31, 0x8))
	);
}

bool mul_row_test(
	size_t num_bytes,
	mul_row_proc proc)
{
	return test_method(
		num_bytes,
		proc,
		gfarith::mul(0x31, 0x8)
	);
}

int main()
{
	bool results[] = {
		mul_add_row_test(32,     &adv::mul_add_row),
		mul_add_row_test(666,    &adv::mul_add_row),
		mul_add_row_test(1,      &adv::mul_add_row),
		mul_add_row_test(519217, &adv::mul_add_row),
		mul_add_row_test(32,     &ssse3::mul_add_row),
		mul_add_row_test(640,    &ssse3::mul_add_row),
		mul_add_row_test(1024,   &ssse3::mul_add_row),
		mul_add_row_test(519216, &ssse3::mul_add_row),
		mul_add_row_test(32,     &avx2::mul_add_row),
		mul_add_row_test(640,    &avx2::mul_add_row),
		mul_add_row_test(1024,   &avx2::mul_add_row),
		mul_add_row_test(519200, &avx2::mul_add_row),
		mul_row_test(32,     &adv::mul_row),
		mul_row_test(666,    &adv::mul_row),
		mul_row_test(1,      &adv::mul_row),
		mul_row_test(519217, &adv::mul_row),
		mul_row_test(32,     &ssse3::mul_row),
		mul_row_test(640,    &ssse3::mul_row),
		mul_row_test(1024,   &ssse3::mul_row),
		mul_row_test(519216, &ssse3::mul_row),
		mul_row_test(32,     &avx2::mul_row),
		mul_row_test(640,    &avx2::mul_row),
		mul_row_test(1024,   &avx2::mul_row),
		mul_row_test(519200, &avx2::mul_row),
	};

	const size_t cnt = std::accumulate(
		std::begin(results),
		std::end(results),
		size_t(0)
	);
	const size_t expected = std::size(results);
	
	if (cnt == expected)
		return 0;
	return 1;
}
