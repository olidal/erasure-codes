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
#endif

	using namespace boost::numeric;

	void matrix_mul(
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

				for (size_t i = 0; i < num_bytes; ++i)
				{
					out[i] = add(out[i], mul(in[i], val));
				}
			}
		}
	}

	void encode(
		const encode_parameters& params,
		const uint8_t* const* shards,
		uint8_t* const* parity)
	{
		matrix mat = ublas::subrange(build_matrix(params.n, params.k), params.k, params.n, 0, params.k);

		matrix_mul(
			mat,
			shards,
			parity,
			params.k,
			params.n - params.k,
			params.data_size);
	}

	bool recover(
		const encode_parameters& params,
		uint8_t* const* shards,
		const bool* present)
	{
		uint8_t n_present =
			std::accumulate(
				present,
				present + params.k,
				uint8_t(0));

		if (n_present == params.k)
			// All the data shards are present
			// no need to do any more work
			return true;

		n_present =
			std::accumulate(
				present + params.k,
				present + params.n,
				n_present);

		if (n_present < params.k)
			// Not enough shards available
			// to recover the data
			return false;

		uint8_t** subshards = (uint8_t**)stackalloc(
			sizeof(uint8_t*) * params.k);

		uint8_t** outputs = (uint8_t**)stackalloc(
			sizeof(uint8_t*) * (params.n - params.k));

		matrix m = build_matrix(params.n, params.k);
		matrix decodemat = matrix{ params.k, params.k };

		for (size_t i = 0, j = 0; i < params.n; ++i)
		{
			if (present[i])
			{
				ublas::row(decodemat, j) = ublas::row(m, i);
				subshards[j] = shards[i];
				++j;
			}
		}

		inverse(decodemat);

		size_t n_outputs = 0;
		for (size_t i = 0, j = 0; i < params.k; ++i)
		{
			if (!present[i])
			{
				outputs[j] = shards[i];
				ublas::row(decodemat, j) = ublas::row(decodemat, i);
				++j;
			}
			n_outputs = j;
		}

		decodemat = ublas::subrange(decodemat, 0, n_outputs, 0, params.k);
		
		matrix_mul(
			decodemat,
			subshards,
			outputs,
			params.k,
			n_outputs,
			params.data_size);

		stackfree(subshards);
		stackfree(outputs);

		return true;
	}

}
