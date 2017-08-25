#include "rs_encoder.h"
#include "encoder_interface.h"

#include <boost/numeric/ublas/matrix_proxy.hpp>

#include <numeric>

namespace erasure
{
#ifdef ERASURE_NO_ALLOCA
#	define stackalloc(size) malloc(size)
#	define stackfree(ptr) free(ptr)
#else
#	define stackalloc(size) alloca(size)
#	define stackfree(ptr);
	// Use this to disable returned pointer checks
	// for alloca since it should never be null in
	// cases other than n_data == n_shards
#	define STACKALLOC_IS_ALLOCA
#endif

	using namespace boost::numeric;

	struct rs_encoder
	{
		matrix coding_mat;
		size_t data_size;
		uint8_t n_data;
		uint8_t n_shards;
		uint8_t n_parity;
	};

	bool validate_args(
		rs_encoder* encoder,
		const uint8_t* const* shards,
		const uint8_t* const* parity)
	{
		if (!encoder || !shards || !parity)
			return false;

		for (size_t i = 0; i < encoder->n_data; ++i)
		{
			if (!shards[i])
				return false;
		}

		for (size_t i = 0; i < encoder->n_parity; ++i)
		{
			if (!parity[i])
				return false;
		}

		return true;
	}
	bool validate_args(
		rs_encoder* encoder,
		const uint8_t* const* shards)
	{
		if (!encoder || !shards)
			return false;

		for (size_t i = 0; i < encoder->n_shards; ++i)
		{
			if (!shards[i])
				return false;
		}

		return true;
	}

	rs_encoder* create_encoder(
		const encode_parameters& params,
		encoder_flags flags)
	{
		// Number of data shards must be less than
		// the total number of shards
		if (params.k > params.n)
			return nullptr;

		// There is no use in making an encoder for
		// data with a size of 0.
		if (params.data_size == 0)
			return nullptr;

		rs_encoder* encoder = new(std::nothrow) rs_encoder;

		if (!encoder)
			// If unable to allocate an encoder
			// then return null
			return nullptr;

		encoder->coding_mat = build_matrix(params.n, params.k);
		encoder->n_data = params.k;
		encoder->n_shards = params.n;
		encoder->n_parity = params.n - params.k;
		encoder->data_size = params.data_size;

		return encoder;
	}

	void destroy_encoder(rs_encoder* encoder)
	{
		delete encoder;
	}

	namespace
	{
		matrix build_encode_matrix(
			rs_encoder* encoder,
			const bool* should_encode,
			size_t mat_sz)
		{
			matrix mat{
				mat_sz,
				encoder->n_data
			};

			for (size_t i = 0, j = 0; i < encoder->n_parity; ++i)
			{
				if (should_encode[i])
				{
					ublas::noalias(ublas::row(mat, j))
						= ublas::row(encoder->coding_mat, i + encoder->n_data);

					++j;
				}
			}
		}
	}

	error_code encode_partial(
		rs_encoder* encoder,
		const uint8_t* const* shards,
		uint8_t* const* parity,
		const bool* should_encode)
	{
		if (!validate_args(encoder, shards, parity) || !should_encode)
			return INVALID_ARGUMENTS;

		size_t mat_sz = std::accumulate(should_encode,
			should_encode + encoder->n_parity, 0);

		if (mat_sz == 0)
			return SUCCESS;

		matrix mat = build_encode_matrix(encoder, should_encode, mat_sz);

		matrix_mul(
			mat,
			shards,
			parity,
			encoder->n_data,
			encoder->n_parity,
			encoder->data_size);

		return SUCCESS;
	}

	error_code encode(
		rs_encoder* encoder,
		const uint8_t* const* shards,
		uint8_t* const* parity)
	{
		bool* should_encode = (bool*)stackalloc(
			sizeof(bool) * (encoder->n_parity));

		if (!should_encode)
			return INTERNAL_ERROR;

		std::memset(should_encode, 1, encoder->n_parity);

		// Forward to encode_partial and indicate
		// that all shards should be encoded
		error_code err = encode_partial(
			encoder,
			shards,
			parity,
			should_encode);

		stackfree(should_encode);

		return err;
	}

	error_code recover_data(
		rs_encoder* encoder,
		uint8_t* const* shards,
		const bool* present)
	{
		if (!validate_args(encoder, shards) || !present)
			return INVALID_ARGUMENTS;

		uint8_t n_present =
			std::accumulate(
				present,
				present + encoder->n_shards,
				uint8_t(0));

		if (n_present < encoder->n_data)
			// Not enough shards available
			// to recover the data
			return RECOVER_FAILED;

		recover_stream* stream = create_recover_stream(encoder, present);

		error_code err = stream_recover_data(stream, shards);

		destroy_stream(stream);

		return err;
	}

	error_code recover(
		rs_encoder* encoder,
		uint8_t* const* shards,
		const bool* present)
	{
		error_code err = recover_data(encoder, shards, present);
		if (err != SUCCESS)
			// Couldn't recover the data,
			// so we can't recover parity
			return err;

		bool* should_encode = (bool*)stackalloc(sizeof(bool) * encoder->n_parity);

#ifndef STACKALLOC_IS_ALLOCA
		if (!should_encode) return INTERNAL_ERROR;
#endif

		for (size_t i = 0; i < encoder->n_parity; ++i)
		{
			should_encode[i] = !present[i + encoder->n_data];
		}

		// Re-encode the missing parity shards
		err = encode_partial(
			encoder,
			shards,
			shards + encoder->n_data,
			present + encoder->n_data);

		stackfree(should_encode);

		return err;
	}
}
