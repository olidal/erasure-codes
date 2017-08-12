#pragma once

#include "matrix.h"

namespace erasure
{
	void code_some_shards(
		const matrix& mat_rows,
		const uint8_t* const * inputs,
		uint8_t* const * outputs,
		size_t n_inputs,
		size_t n_outputs,
		size_t data_size);

	/*
		Preconditions:
			No alignment or size restrictions.
	*/
	void encode_basic(
		const matrix& mat_rows,
		const uint8_t* const * inputs,
		uint8_t* const * outputs,
		size_t n_inputs,
		size_t n_outputs,
		size_t data_size);
}