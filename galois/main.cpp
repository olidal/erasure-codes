#include "encoder.h"

#include <random>

using galois::matrix;
using galois::symbol;

static constexpr size_t n = 6;
static constexpr size_t k = 4;
static constexpr size_t shard_size = k;

bool present[n] = {
	true,
	false,
	true,
	false,
	true,
	true
};

int main()
{
	symbol data[shard_size * n];
	symbol data2[shard_size * n];

	std::uniform_int_distribution<unsigned> dist{ 0, 256 };
	std::mt19937 eng;

	for (size_t i = 0; i < shard_size * k; ++i)
	{
		data[i] = '0' + (uint8_t)i;
	}

	symbol* ptrs[n];

	for (size_t i = 0; i < n; ++i)
		ptrs[i] = data + shard_size * i;

	galois::encode<n, k>(ptrs, ptrs + k, shard_size);

	memcpy(data2, data, sizeof(data));

	for (size_t i = 0; i < n; ++i)
	{
		if (!present[i / 3])
		{
			data[i] = 0xFF;
		}
	}

	galois::recover<n, k>(ptrs, present, shard_size);
}
