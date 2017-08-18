#include "encoder_interface.h"

#include <intrin.h>

namespace erasure
{
	struct feature_bits
	{
		uint32_t sse3  : 1;
		uint32_t sse4  : 1;
		uint32_t sse41 : 1;
		uint32_t sse42 : 1;
		uint32_t sse4a : 1;
	};

#ifdef __GNUC__

#endif


	void matrix_mul(
		const matrix& mat,
		const uint8_t* const* inputs,
		uint8_t* const* outputs,
		size_t n_inputs,
		size_t n_outputs,
		size_t num_bytes)
	{
		matrix_mul_adv(mat, inputs, outputs, n_inputs, n_outputs, num_bytes);
	}
}
