#pragma once

#include "matrix.h"

namespace erasure
{
	void matrix_mul(
		const matrix& mat,
		const uint8_t* const* shards,
		uint8_t* const* outputs,
		uint8_t n_ouputs,
		size_t num_bytes);
	
	void matrix_mul_base(
		const matrix& mat,
		const uint8_t* const* shards,
		uint8_t* const* outputs,
		uint8_t n_ouputs,
		size_t num_bytes);
}