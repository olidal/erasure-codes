#pragma once

#include "matrix.h"

namespace galois
{
	template<size_t n, size_t k>
	void encode(
		const symbol* const shards[k], 
		symbol* parity[n-k], 
		size_t shard_size)
	{
		matrix<k, k> data_mat;
		matrix<n, k> encoding_mat = build_matrix<n, k>();
		matrix<n, k> encoded;

		for (size_t i = 0; i < shard_size; i += k)
		{
			for (size_t j = 0; j < k; ++j)
			{
				std::memcpy(data_mat[j], shards[j] + i, k * sizeof(symbol));
			}

			encoded = encoding_mat * data_mat;

			for (size_t j = 0; j < n - k; ++j)
			{
				std::memcpy(parity[j] + i, encoded[j + k], k * sizeof(symbol));
			}
		}
	}

	template<size_t n, size_t k>
	bool recover(
		symbol* shards[n],
		bool present[n],
		size_t shard_size)
	{
		size_t n_present = 0;
		for (size_t i = 0; i < k; ++i)
			n_present += present[i];

		if (n_present == k)
			// All data shards are present
			// no recovery required
			return true;

		for (size_t i = k; i < n; ++i)
			n_present += present[i];

		matrix<k, k> decode_mat;
		matrix<k, k> encoding_submat;
		size_t indices[k];
		
		{
			matrix<n, k> encoding_mat = build_matrix<n, k>();

			size_t j = 0;
			for (size_t i = 0; i < n && j < k; ++i)
			{
				if (present[i])
				{
					indices[j] = i;
					std::memcpy(
						encoding_submat[j], 
						encoding_mat[i], 
						k * sizeof(symbol));
					++j;
				}
			}
			
			if (j < k)
				// We don't have enough shards to recover the data
				return false;

			// Get the inverse of the matrix
			encoding_submat = encoding_submat.inverse();
		}

		for (size_t i = 0; i < shard_size; i += k)
		{
			for (size_t j = 0; j < k; ++j)
			{
				std::memcpy(
					decode_mat[j], 
					shards[indices[j]] + i,
					k * sizeof(symbol));
			}

			auto data = encoding_submat * decode_mat;

			for (size_t j = 0; j < k; ++j)
			{
				std::memcpy(
					shards[j] + i, 
					data[j], 
					k * sizeof(symbol));
			}
		}

		return true;
	}
}
