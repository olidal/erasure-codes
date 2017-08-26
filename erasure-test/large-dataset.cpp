#include "rs_encoder.h"

using namespace erasure;

#include <random>
#include <cstdlib>

static constexpr size_t k = 64;
static constexpr size_t n = 80;
static constexpr size_t data_size = 1024 * 1024;

uint8_t* ptrs[n];
bool present[n];

uint8_t* data = (uint8_t*)_mm_malloc(data_size * k, 64);
uint8_t* result = (uint8_t*)_mm_malloc(data_size * k, 64);
uint8_t* parity = (uint8_t*)_mm_malloc(data_size * (n - k), 64);

// Default value from mt19937 argument default value
void generate_data(unsigned seed = 5489u)
{
	std::uniform_int_distribution<unsigned> dist{ 0, 255 };
	std::mt19937 eng{ seed };

	for (size_t i = 0; i < sizeof(data) / sizeof(uint8_t); ++i)
	{
		data[i] = dist(eng);
	}
}

void generate_ptrs()
{
	size_t i, j;
	for (i = 0; i < k; ++i)
	{
		ptrs[i] = data + data_size * i;
	}
	for (j = 0; i < n; ++i, ++j)
	{
		ptrs[i] = parity + data_size * j;
	}

	for (size_t m = 0; m < n; ++m)
	{
		present[m] = (m % 5) != 0;
	}
}

int main()
{
	generate_data();
	generate_ptrs();

	encode_parameters params = { n, k, data_size };

	rs_encoder* encoder = create_encoder(params);

	encode(encoder, ptrs, ptrs + k);

	memcpy(result, data, sizeof(uint8_t) * data_size * k);

	for (size_t i = 0; i < k; ++i)
	{
		if (!present[i])
		{
			memset(ptrs[i], 0, data_size * sizeof(uint8_t));
		}
	}

	recover(encoder, ptrs, present);

	destroy_encoder(encoder);

	if (memcmp(data, result, sizeof(uint8_t) * data_size * k) != 0)
		return 1;
	return 0;
}
