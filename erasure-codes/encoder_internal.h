#ifndef ERASURE_ENCODER_INTERNAL_H
#define ERASURE_ENCODER_INTERNAL_H

#include <cstdint>
#include "matrix.h"

namespace erasure
{
	alignas(64) const extern uint8_t lohi_table[256][2][16];

	namespace adv
	{
		void mul_add_row(uint8_t val, const uint8_t* in, uint8_t* out, size_t num_bytes);
		void mul_row(uint8_t val, const uint8_t* in, uint8_t* out, size_t num_bytes);
	}
	
	namespace ssse3
	{
		void mul_add_row(uint8_t val, const uint8_t* in, uint8_t* out, size_t num_bytes);
		void mul_row(uint8_t val, const uint8_t* in, uint8_t* out, size_t num_bytes);
	}
	namespace avx2
	{
		void mul_add_row(uint8_t val, const uint8_t* in, uint8_t* out, size_t num_bytes);
		void mul_row(uint8_t val, const uint8_t* in, uint8_t* out, size_t num_bytes);
	}

	/* Selects the fastest method based on
	   alignment of the input and output
	   pointers.
	*/
	void matrix_mul(
		const matrix& mat,
		const uint8_t* const* inputs,
		uint8_t* const* outputs,
		size_t n_inputs,
		size_t n_outputs,
		size_t num_bytes);

	/*
	Preconditions:
	   No alignment or size restrictions.
	*/
	void matrix_mul_basic(
		const matrix& mat,
		const uint8_t* const* inputs,
		uint8_t* const* outputs,
		size_t n_inputs,
		size_t n_outputs,
		size_t num_bytes);
	
	/*
	Preconditions:
	   No alignment or size restrictions.
	*/
	void matrix_mul_adv(
		const matrix& mat,
		const uint8_t* const* inputs,
		uint8_t* const* outputs,
		size_t n_inputs,
		size_t n_outputs,
		size_t num_bytes);

	/*
	Preconditions:
	   Inputs 16 byte aligned.
	*/
	void matrix_mul_sse(
		const matrix& mat,
		const uint8_t* const* inputs,
		uint8_t* const* outputs,
		size_t n_inputs,
		size_t n_outputs,
		size_t num_bytes);

	/*
	Preconditions:
		None
	*/
	void matrix_mul_avx2(
		const matrix& mat,
		const uint8_t* const* inputs,
		uint8_t* const* outputs,
		size_t n_inputs,
		size_t n_outputs,
		size_t num_bytes);

	typedef void(*matrix_mul_proc)(
		const matrix& mat,
		const uint8_t* const* inputs,
		uint8_t* const* outputs,
		size_t n_inputs,
		size_t n_outputs,
		size_t num_bytes);
	
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

#endif
