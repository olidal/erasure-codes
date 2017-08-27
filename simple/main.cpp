
#include "encoder_internal.h"
#include "rs_encoder.h"

#include <intrin.h>
#include <iostream>

using erasure::rs_encoder;
using erasure::create_encoder;
using erasure::destroy_encoder;
using erasure::matrix;

void encode(
	rs_encoder* encoder,
	const uint8_t* const* shards,
	uint8_t* const* parity)
{
	matrix encode = encoder->coding_mat.submatrix(
		encoder->n_data, encoder->n_shards,
		0, encoder->n_data);

	encoder->mul_proc(
		encode,
		shards,
		parity,
		encoder->n_data,
		encoder->n_parity,
		encoder->data_size
	);
}

void recover_data(
	rs_encoder* encoder,
	uint8_t* const* shards,
	const bool* present)
{
	matrix decode = matrix(encoder->n_data, encoder->n_data);

	uint8_t** inputs = (uint8_t**)alloca(encoder->n_data * sizeof(uint8_t*));
	uint8_t** outputs = (uint8_t**)alloca(encoder->n_parity * sizeof(uint8_t*));

	for (size_t i = 0, j = 0; i < encoder->n_shards && j < encoder->n_data; ++i)
	{
		if (present[i])
		{
			decode.row(j) = encoder->coding_mat.row(i);
			inputs[j] = shards[i];
			++j;
		}
	}

	inverse(decode);

	size_t n_outputs = 0;
	for (size_t i = 0; i < encoder->n_data; ++i)
	{
		if (!present[i])
		{
			decode.row(n_outputs) = decode.row(i);
			outputs[n_outputs] = shards[i];
			++n_outputs;
		}
	}

	encoder->mul_proc(
		decode,
		inputs,
		outputs,
		encoder->n_data,
		n_outputs,
		encoder->data_size
	);
}

constexpr size_t data_size = 1;

uint8_t** calc_shards(uint8_t* buffer, size_t n, size_t k, size_t data_size)
{
	uint8_t** ptrs = new uint8_t*[n];

	for (size_t i = 0; i < n; ++i)
	{
		ptrs[i] = buffer + data_size * i;
	}

	return ptrs;
}
uint8_t** calc_parity(uint8_t* buffer, size_t n, size_t k, size_t data_size)
{
	uint8_t** ptrs = new uint8_t*[n - k];

	for (size_t i = 0; i < n - k; ++i)
	{
		ptrs[i] = buffer + data_size * (k + i);
	}

	return ptrs;
}
bool* calc_present(size_t n, size_t k)
{
	bool* present = new bool[n];

	for (size_t i = 0; i < n - k; ++i)
		present[i] = false;
	for (size_t i = n - k; i < n; ++i)
		present[i] = true;

	return present;
}

bool run_test(uint8_t n, uint8_t k)
{
	uint8_t* buffer = new uint8_t[n * data_size * 2];
	uint8_t* result = buffer + n * data_size;

	uint8_t**shards = calc_shards(buffer, n, k, data_size);
	uint8_t*const* parity = calc_parity(buffer, n, k, data_size);
	const bool* present = calc_present(n, k);

	std::memset(buffer, 0x4F, n * data_size);

	rs_encoder* encoder = create_encoder({ n, k, data_size }, erasure::USE_REF_IMPL);

	::encode(encoder, shards, parity);

	memcpy(result, buffer, n * data_size);

	for (size_t i = 0; i < n; ++i)
	{
		if (!present[i])
			memset(shards[i], 0, data_size);
	}

	::recover_data(encoder, shards, present);
	::encode(encoder, shards, parity);

	destroy_encoder(encoder);

	bool val = memcmp(result, buffer, data_size * k) == 0;

	delete[] buffer;
	delete[] shards;
	delete[] parity;
	delete[] present;

	return val;
}

int main()
{
	run_test(8, 4);

	for (size_t n = 1; n < 256; ++n)
	{
		for (size_t k = 1; k < n; ++k)
		{
			if (!run_test(n, k))
			{
				std::cout << "Error with n=" << n << ", k=" << k << std::endl;
			}
		}
	}

	return 0;
}
