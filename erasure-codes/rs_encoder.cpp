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
			params.n - params.k,
			params.data_size);
	}

	bool recover(
		const encode_parameters& params,
		uint8_t* const* shards,
		const bool* present)
	{
		typedef ublas::matrix_row<matrix> mat_row;

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

		uint8_t** shardptrs = (uint8_t**)stackalloc(sizeof(uint8_t*) * params.k);

		matrix enc_mat = build_matrix(params.n, params.k);
		matrix enc_submat{ params.k, params.k };

		for (size_t i = 0, j = 0; j < params.k; ++i)
		{
			// This should have been verified at the 
			// start of the function. If those checks
			// are deleted then this will fail if it
			// is not possible to recover the data
			assert(i < params.n);
			if (present[i])
			{
				shardptrs[j] = shards[i];
				mat_row(enc_submat, j) = mat_row(enc_mat, i);
				++j;
			}
		}

		{
			bool result = inverse(enc_submat);
			// If the matrix is not inverible then 
			// something is wrong with build_matrix.
			// This should not happen.
			assert(result);
		}

		matrix_mul(enc_submat, shardptrs, shards, params.k, params.data_size);

		stackfree(shardptrs);

		return true;
	}

}
