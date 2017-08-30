#include "liberasure.h"

#include <random>

static constexpr size_t k = 16;
static constexpr size_t n = 20;
static constexpr size_t data_size = k * 128;

alignas(64) uint8_t data[data_size * k];
alignas(64) uint8_t result1[data_size * k];
alignas(64) uint8_t result2[data_size * k];

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

void run_test(erasure_encoder_flags flag, void* data_ptr)
{
	generate_data();
	generate_ptrs();

	erasure_encoder_parameters params = { n, k, data_size };

	erasure_encoder* encoder = erasure_create_encoder(&params, flag);

	erasure_encode(encoder, ptrs, ptrs + k);

	memcpy(data_ptr, data, sizeof(data));

	for (size_t i = 0; i < k; ++i)
	{
		if (!present[i])
		{
			memset(ptrs[i], 0, data_size * sizeof(uint8_t));
		}
	}

	erasure_recover(encoder, ptrs, present);

	erasure_destroy_encoder(encoder);
}

int main()
{
	run_test(ERASURE_FORCE_REF_IMPL, result1);
	run_test(ERASURE_FORCE_AVX2_IMPL, result2);

	if (memcmp(result1, result2, sizeof(data)) != 0)
		return 1;
	return 0;
}
