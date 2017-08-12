#include "encoder_interface.h"

namespace erasure
{
	/*
		Reference implementation for encoding.
		This is the slowest implmentation.
	*/
	void encode_basic(
		const matrix& mat_rows,
		const uint8_t* const * inputs,
		uint8_t* const * outputs,
		size_t n_inputs,
		size_t n_outputs,
		size_t data_size)
	{
		for (size_t c = 0; c < n_inputs; ++c)
		{
			const uint8_t* in = inputs[c];
			if (c == 0)
			{
				for (size_t r = 0; r < n_outputs; ++r)
				{
					uint8_t* out = outputs[r];
					uint8_t val = mat_rows(r, c).value;
					for (size_t i = 0; i < data_size; ++i)
					{
						out[i] = mul(in[i], val);
					}
				}
			}
			else
			{
				for (size_t r = 0; r < n_outputs; ++r)
				{
					uint8_t* out = outputs[r];
					uint8_t val = mat_rows(r, c).value;
					for (size_t i = 0; i < data_size; ++i)
					{
						out[i] = add(out[i], mul(in[i], val));
					}
				}

			}
		}
	}
}

