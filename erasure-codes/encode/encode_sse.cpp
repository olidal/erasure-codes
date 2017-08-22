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
				__m128i lomask = _mm_set1_epi8(0x0F);
				__m128i himask = _mm_set1_epi8((char)0xF0);

				for (size_t i = 0; i < num_bytes; i += sizeof(__m128i))
				{
					__m128i in_vals = _mm_load_si128((const __m128i*)(in + i));
					__m128i idx1 = _mm_and_si128(in_vals, lomask);
					__m128i idx2 = _mm_and_si128(in_vals, himask);
					idx2 = _mm_srli_epi64(idx2, 4);
					__m128i lo_vals = _mm_shuffle_epi8(idx1, lo);
					__m128i hi_vals = _mm_shuffle_epi8(idx2, hi);
					__m128i result = _mm_xor_si128(lo_vals, hi_vals);
					result = _mm_xor_si128(result, _mm_load_si128((const __m128i*)(in + i)));
					uint8_t rval = mul(val, in[i]);
					_mm_store_si128((__m128i*)(out + i), result);
				}
			}
		}
	}
}
