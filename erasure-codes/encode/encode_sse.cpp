#include "encoder_interface.h"

#include <intrin.h>

namespace erasure
{
	void matrix_mul_sse(
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
			// Must be 16-byte aligned
			uint8_t* out = outputs[r];

			for (size_t c = 0; c < n_inputs; ++c)
			{
				// Must be 16-byte aligned
				const uint8_t* in = inputs[c];
				const uint8_t val = mat(r, c).value;
				__m128i lo = _mm_load_si128((const __m128i*)lohi_table[val][0]);
				__m128i hi = _mm_load_si128((const __m128i*)lohi_table[val][1]);
				__m128i mask = _mm_set1_epi8(0x0F);

				for (size_t i = 0; i < num_bytes; i += sizeof(__m128i))
				{
					// in_val = in[i]
					__m128i in_vals = _mm_load_si128((const __m128i*)(in + i));
					// idx1 = in_val & 0xF
					__m128i idx1 = _mm_and_si128(in_vals, mask);
					// idx2 = (in_val >> 4) & 0xF
					__m128i shifted = _mm_srli_epi64(in_vals, 4);
					__m128i idx2 = _mm_and_si128(shifted, mask);
					// lo_val = lo[idx1]
					__m128i lo_vals = _mm_shuffle_epi8(lo, idx1);
					// hi_vals = hi[idx1]
					__m128i hi_vals = _mm_shuffle_epi8(hi, idx2);
					// result = lo_val ^ hi_val
					__m128i result = _mm_xor_si128(lo_vals, hi_vals);
					// out[i] ^= result
					result = _mm_xor_si128(result, _mm_load_si128((__m128i*)(out + i)));
					_mm_store_si128((__m128i*)(out + i), result);
				}
			}
		}
	}
}
