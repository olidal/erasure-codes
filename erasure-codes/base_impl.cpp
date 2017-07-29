
#include "matrix.h"

#include <numeric>
#include <vector>

static constexpr size_t n = 20;
static constexpr size_t k = 16;
static constexpr size_t node_bytes = 1024 * 1024 * 64 / k;

using namespace erasure;

namespace base_impl
{
	static matrix vandermonde(size_t rows, size_t cols)
	{
		matrix result = matrix(rows, cols);
		for (uint8_t r = 0; r < rows; ++r)
		{
			for (uint8_t c = 0; c < cols; ++c)
			{
				result(r, c) = symbol_t::exp(r, c);
			}
		}
		return result;
	}
	static matrix build_matrix()
	{
		matrix vm = vandermonde(n, k);

		return vm * inverse(vm.submatrix(k, k));
	}

	void encode_single(
		const matrix& mat, 
		const symbol_t data[k], 
		symbol_t output[n - k])
	{
		for (size_t c = 0; c < k; ++c)
		{
			auto in = data[c];

			for (size_t row = 0; row < n - k; ++row)
			{
				if (c == 0)
					output[row] = in * mat(row, c);
				else
					output[row] += in * mat(row, c);
			}
		}
	}

	void encode(const uint8_t* data, uint8_t* parity)
	{
		matrix mat = build_matrix();

		for (size_t i = 0; i < node_bytes; ++i)
		{
			symbol_t vals[k];
			symbol_t par[n - k];

			for (size_t j = 0; j < k; ++j)
			{
				vals[j] = data[i + j * node_bytes];
			}

			encode_single(mat, vals, par);

			for (size_t j = 0; j < n - k; ++j)
			{
				parity[i + j * node_bytes] = par[j].value;
			}
		}
	}

	enum reconstruct_status
	{
		RECONSTRUCT_OK,
		RECONSTRUCT_ERR_SHARDS_INVALID,
		RECONSTRUCT_ERR_TOO_FEW_BLOCKS
	};

	void reconstruct_single(uint8_t blocks[n], bool present[n])
	{
		matrix mat = build_matrix();
		matrix submat = matrix(k, k);
		uint8_t subblocks[k];

		for (size_t submatrow = 0, matrow = 0; matrow < n && submatrow < k; matrow++)
		{
			if (present[matrow])
			{
				for (size_t c = 0; c < k; ++c)
					submat(submatrow, c) = mat(matrow, c);

				subblocks[submatrow] = blocks[matrow];
				submatrow++;
			}
		}

		matrix decodemat = inverse(submat);
		
		uint8_t output[n - k];
		matrix mat_rows = matrix(n - k, k);

		for (size_t i = 0, j = 0; i < k; ++i)
		{
			if (!present[i])
			{
				memcpy(mat_rows.row(j), decodemat.row(i), k);
				j++;
			}
		}

		encode_single(mat_rows, (symbol_t*)subblocks, (symbol_t*)output);

		for (size_t i = k, j = 0; i < n; ++i)
		{

		}
	}

	reconstruct_status reconstruct_data(uint8_t** blocks, bool* present)
	{
		size_t n_data = std::accumulate(present, present + k, 0);
		size_t n_parity = std::accumulate(present + k, present + n, 0);

		if (n_data == k)
			// All data is present
			return RECONSTRUCT_OK;

		if (n_data + n_parity < k)
			// Not enough fragments to reconstruct the data
			return RECONSTRUCT_ERR_TOO_FEW_BLOCKS;


	}
}

