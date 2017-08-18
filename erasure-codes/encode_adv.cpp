#include "encoder_interface.h"

namespace erasure
{
	void matrix_mul_adv(
		const matrix& mat,
		const uint8_t* const* inputs,
		uint8_t* const* outputs,
		size_t n_inputs,
		size_t n_outputs,
		size_t num_bytes)
	{
		for (size_t i = 0; i < n_outputs; ++i)
			std::memset(outputs[i], 0, num_bytes);

		for (size_t r = 0; r < n_outputs; ++r)
		{
			uint8_t* out = outputs[r];

			for (size_t c = 0; c < n_inputs; ++c)
			{
				uint8_t val = mat(r, c).value;
				const uint8_t* in = inputs[c];
				const uint8_t* lo = lohi_table[val][0];
				const uint8_t* hi = lohi_table[val][1];

				for (size_t i = 0; i < num_bytes; ++i)
				{
					uint8_t in_val = in[i];
					uint8_t idx1 = in_val & 0xF;
					uint8_t idx2 = in_val >> 4;
					uint8_t lo_val = lo[idx1];
					uint8_t hi_val = hi[idx2];
					uint8_t result1 = lo_val ^ hi_val ^ out[i];
					uint8_t result2 = mul(val, in_val) ^ out[i];
					out[i] = result1;
				}
			}
		}
	}
}