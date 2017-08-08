#include "encoder.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <random>

using namespace galois;

symbol* make_random(size_t size, size_t seed)
{
	std::uniform_int_distribution<int> dist{ 0, 255 };
	std::mt19937 eng{ (unsigned int)seed };

	symbol* buffer = new symbol[size];

	for (size_t i = 0; i < size; ++i)
	{
		buffer[i] = dist(eng);
	}

	return buffer;
}

TEST_CASE("Decodes properly with missing segments", "[encoder]")
{
	constexpr size_t data_size = 8;

	symbol* rand_data = make_random(data_size * 8, 0x1379adaf);
	symbol* copy = new symbol[data_size * 8];

	memcpy(copy, rand_data, sizeof(symbol) * data_size * 8);

	symbol *shards[10], parity_data[data_size * 2];
	bool present[] = { 1, 0, 0, 1, 1, 1, 1, 1, 1, 1 };
	
	for (size_t i = 0; i < 8; ++i)
	{
		shards[i] = rand_data + i * data_size;
	}
	for (size_t i = 8; i < 10; ++i)
	{
		shards[i] = parity_data + (i - 8) * data_size;
	}
	
	encode<10, 8>(shards, shards + 8, data_size);

	for (size_t i = 0; i < 8; ++i)
	{
		if (!present[i])
			memset(rand_data + data_size * i, 0, sizeof(symbol) * data_size);
	}

	recover<10, 8>(shards, present, data_size);

	REQUIRE(memcmp(rand_data, copy, sizeof(symbol) * data_size * 8) == 0);

	delete[] rand_data;
	delete[] copy;
}
