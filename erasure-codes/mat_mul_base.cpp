#include "encoder_interface.h"

namespace erasure
{
	void matrix_mul_base(
		const matrix& mat,
		const uint8_t* const* shards,
		uint8_t* const* outputs,
		uint8_t n_ouputs,
		size_t num_bytes);
}
