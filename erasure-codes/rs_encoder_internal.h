#include "matrix.h"
#include "encoder_interface.h"

#include <cstdint>

namespace erasure
{
	struct rs_encoder
	{
		matrix coding_mat;
		size_t data_size;
		matrix_mul_proc mul_proc;
		uint8_t n_data;
		uint8_t n_shards;
		uint8_t n_parity;
	};
}
