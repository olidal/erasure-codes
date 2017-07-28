#pragma once

#include <armadillo>

#include "galois.h"
#include "matrix.h"

namespace erasure
{
	struct rsparams
	{
		uint8_t n; // Number of nodes
		uint8_t k; // Number of data segments
	};

	class rsencoder
	{
	private:
		typedef galois_int<uint8_t> symbol_t;
		typedef arma::Mat<symbol_t> matrix_t;

		/* Create a Vandermonde matrix, which has the property
		   that any subset of the rows that forms a square 
		   matrix is invertible.		
		*/
		static matrix_t vandermonde(arma::uword rows, arma::uword cols)
		{
			matrix_t result = matrix_t(rows, cols);
			for (uint8_t r = 0; r < rows; ++r)
			{
				for (uint8_t c = 0; c < cols; ++c)
				{
					result(r, c) = symbol_t::exp(r, c);
				}
			}
			return result;
		}

		/* Create the encoding matrix, given the total 
		   number of nodes and the number of data nodes

		   n: Total number of nodes
		   k: Number of data nodes
		*/
		static matrix_t build_matrix(arma::uword k, arma::uword n)
		{
			// Calculate the vandermonde matrix, it could
			// but then the data shards would be changed 
			// after encoding.
			matrix_t vm = vandermonde(n, k);

			matrix_t top = vm.submat(0, 0, k, k);
			return vm * inverse(top);
		}



	public:
		matrix_t matrix;
		rsparams params;

		void decode_missing(
			matrix_t& symbols,
			const bool* present)
		{
			size_t n_present = 0;
			for (size_t i = 0; i < symbols.n_rows; ++i)
				n_present += present[i];

			if (n_present == symbols.n_rows)
				return; // All shards present

			// Not enough symbols to recover
			assert(n_present < params.k);
			
			arma::uvec4 row_indices;
			for (size_t i = 0, j = 0; j < 4; ++i)
			{
				if (present[i])
				{
					row_indices[j] = i;
					++j;
				}
			}

			matrix_t decoded = inverse(symbols.rows(row_indices));

			
		}

		void encode()
		{

		}
	};
}
