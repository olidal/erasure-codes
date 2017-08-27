
#include "rs_encoder.h"
#include <immintrin.h>
#include <random>
#include <algorithm>
#include <iostream>

void fill_data(uint8_t*const* data, size_t k, size_t data_size, unsigned seed = 5489u)
{
	std::uniform_int_distribution<unsigned> dist{ 0, 255 };
	std::mt19937 eng(seed);

	for (size_t j = 0; j < k; ++j)
	{
		for (size_t i = 0; i < data_size; ++i)
		{
			data[j][i] = (uint8_t)dist(eng);
		}
	}
}

uint8_t** build_array(size_t n, size_t data_size, size_t align = 64)
{
	uint8_t** array = new uint8_t*[n];

	for (size_t i = 0; i < n; ++i)
	{
		array[i] = (uint8_t*)_mm_malloc(data_size * sizeof(uint8_t), align);
	}

	return array;
}
void free_array(uint8_t** array, size_t n)
{
	for (size_t i = 0; i < n; ++i)
		_mm_free(array[i]);

	delete[] array;
}
void copy_array(uint8_t** dst, uint8_t** src, size_t n, size_t data_size)
{
	for (size_t i = 0; i < n; ++i)
	{
		std::memcpy(dst[i], src[i], data_size);
	}
}
bool verify(
	uint8_t** to_check,
	uint8_t** copy,
	size_t n,
	size_t data_size)
{
	for (size_t i = 0; i < n; ++i)
	{
		if (std::memcmp(to_check[i], copy[i], data_size) != 0)
			return false;
	}

	return true;
}

void generate_present(bool* present, size_t n, size_t k)
{
	size_t recoverable = n - k;
	for (size_t m = 0; m < n; ++m)
	{
		present[m] = (m % recoverable) != 0;
	}
}

using namespace erasure;

bool run_test(
	size_t n,
	size_t k,
	size_t data_size,
	encoder_flags flags = DEFAULT,
	size_t align = 64,
	unsigned seed = 5489u)
{
	encode_parameters params = { (uint8_t)n, (uint8_t)k, data_size };
	rs_encoder* encoder = create_encoder(params, flags);
	bool* present = new bool[n];

	uint8_t** copy1 = build_array(n, data_size, align);
	uint8_t** copy2 = build_array(n, data_size, align);

	generate_present(present, n, k);

	fill_data(copy1, k, data_size, seed);

	encode(encoder, copy1, copy1 + k);

	copy_array(copy2, copy1, n, data_size);

	for (size_t k = 0; k < n; ++k)
		if (!present)
			std::memset(copy1, 0, data_size);

	recover(encoder, copy1, present);

	auto result = verify(copy1, copy2, n, data_size);

	destroy_encoder(encoder);
	free_array(copy1, n);
	free_array(copy2, n);
	delete[] present;

	return result;
}

int main()
{
	const size_t data_size = 145;
	int result = 0;

	for (size_t n = 5; n < 10; ++n)
	{
		for (size_t k = 1; k < n; ++k)
		{
			if (!run_test(n, k, data_size))
			{
				result = 1;
				std::cout << '{' << n << ", " << k << '}' << std::endl;
			}
		}
	}

	return result;
}
