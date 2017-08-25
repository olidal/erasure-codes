#include "encoder_interface.h"

#include <intrin.h>

namespace erasure
{
	namespace
	{
		void mul_add_row(uint8_t val, const uint8_t* in, uint8_t* out, size_t num_bytes)
		{
			const __m256i mask = _mm256_set1_epi8(0x0F);
			// Load the entire lookup table into the lo register
			__m256i lo = _mm256_load_si256((const __m256i*)lohi_table[val][0]);
			// Move the hi part of the table into it's own register
			// and duplicate it across the hi and lo halfs of the register
			__m256i hi = _mm256_permute2f128_si256(lo, lo, 0b0010001);

			// Duplicate lower half of lo across the whole register
			lo = _mm256_permute2f128_si256(lo, lo, 0);


			for (size_t i = 0; i < num_bytes; i += sizeof(__m256i))
			{
				// in_val = in[i]
				__m256i in_vals = _mm256_loadu_si256((const __m256i*)(in + i));
				// idx1 = in_val & 0xF
				__m256i idx1 = _mm256_and_si256(in_vals, mask);
				// idx2 = (in_val >> 4) & 0xF
				__m256i shifted = _mm256_srli_epi64(in_vals, 4);
				__m256i idx2 = _mm256_and_si256(shifted, mask);
				// lo_val = lo[idx1]
				__m256i lo_vals = _mm256_shuffle_epi8(lo, idx1);
				// hi_val = hi[idx2]
				__m256i hi_vals = _mm256_shuffle_epi8(hi, idx2);
				// result = lo_val ^ hi_val
				__m256i result = _mm256_xor_si256(lo_vals, hi_vals);
				// out[i] ^= result
				result = _mm256_xor_si256(result, _mm256_loadu_si256((__m256i*)(out + i)));
				_mm256_storeu_si256((__m256i*)(out + i), result);
			}
		}
		void mul_row(uint8_t val, const uint8_t* in, uint8_t* out, size_t num_bytes)
		{
			const __m256i mask = _mm256_set1_epi8(0x0F);
			// Load the entire lookup table into the lo register
			__m256i lo = _mm256_load_si256((const __m256i*)lohi_table[val][0]);
			// Move the hi part of the table into it's own register
			// and duplicate it across the hi and lo halfs of the register
			__m256i hi = _mm256_permute2f128_si256(lo, lo, 0b0010001);

			// Duplicate lower half of lo across the whole register
			lo = _mm256_permute2f128_si256(lo, lo, 0);

			for (size_t i = 0; i < num_bytes; i += sizeof(__m256i))
			{
				// in_val = in[i]
				__m256i in_vals = _mm256_loadu_si256((const __m256i*)(in + i));
				// idx1 = in_val & 0xF
				__m256i idx1 = _mm256_and_si256(in_vals, mask);
				// idx2 = (in_val >> 4) & 0xF
				__m256i shifted = _mm256_srli_epi64(in_vals, 4);
				__m256i idx2 = _mm256_and_si256(shifted, mask);
				// lo_val = lo[idx1]
				__m256i lo_vals = _mm256_shuffle_epi8(lo, idx1);
				// hi_val = hi[idx2]
				__m256i hi_vals = _mm256_shuffle_epi8(hi, idx2);
				// result = lo_val ^ hi_val
				__m256i result = _mm256_xor_si256(lo_vals, hi_vals);
				// out[i] = result
				_mm256_storeu_si256((__m256i*)(out + i), result);
			}
		}
	}

	void matrix_mul_avx2(
		const matrix& mat,
		const uint8_t* const* inputs,
		uint8_t* const* outputs,
		size_t n_inputs,
		size_t n_outputs,
		size_t num_bytes)
	{
		assert(num_bytes % 32 == 0);
				
		for (size_t r = 0; r < n_outputs; ++r)
		{
			uint8_t* out = outputs[r];

			for (size_t c = 0; c < n_inputs; ++c)
			{
				const uint8_t* in = inputs[c];

				if (c == 0)
					mul_row(mat(r, c).value, in, out, num_bytes);
				else
					mul_add_row(mat(r, c).value, in, out, num_bytes);
			}
		}

		_mm256_zeroupper();
	}
}
