#include "liberasure.h"


#include <random>

uint8_t data[1024];
uint8_t result[1024];
static constexpr size_t k = 16;
static constexpr size_t n = 20;
static constexpr size_t data_size = sizeof(data) / sizeof(uint8_t) / k;

uint8_t* ptrs[n];
uint8_t parity[data_size * (n - k)];
erasure_bool present[n];

// Default value from mt19937 argument default value
void generate_data(unsigned seed = 5489u)
{
	std::uniform_int_distribution<unsigned> dist{ 0, 255 };
	std::mt19937 eng{ seed };

	for (size_t i = 0; i < sizeof(data) / sizeof(uint8_t); ++i)
	{
		data[i] = 0xAA;// dist(eng);
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
		present[m] = true;
	}
}

int main()
{
	generate_data();
	generate_ptrs();

	erasure_encoder_parameters params = { n, k, data_size };

	erasure_encoder* encoder = erasure_create_encoder(&params, ERASURE_DEFAULT);

	erasure_encode(encoder, ptrs, ptrs + k);

	memcpy(result, data, sizeof(data));

	erasure_recover(encoder, ptrs, present);

	erasure_destroy_encoder(encoder);

	if (memcmp(data, result, sizeof(data)) != 0)
		return 1;
	return 0;
}
