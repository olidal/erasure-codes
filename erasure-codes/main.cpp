#include "galois.h"
#include "interface.h"

#include <random>

using namespace erasure;

typedef galois_field<uint8_t> field;

uint8_t indices[] = {
	0, 3, 4, 5
};

int main()
{
	field::init();

	uint8_t input[num_bytes * k];
	uint8_t output[num_bytes * n];

	uint8_t* pointers[k] = {
		input,
		input + num_bytes,
		input + num_bytes * 2,
		input + num_bytes * 3
	};
	uint8_t* parity_ptrs[n - k] = {
		output + num_bytes * k,
		output + num_bytes * (k + 1)
	};

	std::uniform_int_distribution<uint16_t> dist{ 0, 255 };
	std::mt19937 eng;

	for (size_t i = 0; i < sizeof(input); ++i)
	{
		input[i] = 5;// dist(eng);
	}

	encode(pointers, parity_ptrs);

	uint8_t* output_ptrs[n];
	bool present[n];
	std::memset(present, 0, sizeof(present));
	std::memcpy(output, input, sizeof(input));

	for (size_t i = 0; i < n; ++i)
	{
		output_ptrs[i] = output + num_bytes * i;
	}
	for (size_t i = 0; i < k; ++i)
	{
		present[indices[i]] = true;
	}

	auto r = reconstruct(output_ptrs, present);

	auto v = memcmp(input, output, sizeof(input));
	assert(v == 0);
}
