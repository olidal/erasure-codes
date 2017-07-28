
#include "matrix.h"

static constexpr size_t n = 20;
static constexpr size_t k = 16;
static constexpr size_t node_bytes = 4096;

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

	void encode_single(const symbol_t data[k], symbol_t output[n - k])
	{
		for (size_t c = 0; c < k; ++c)
		{
			auto in = data[c];

			for (size_t row = 0; row < n - k; ++row)
			{
				if (c == 0)
					output[row] = in * c;
				else
					output[row] += in * c;
			}
		}
	}

	void encode(const uint8_t* data, uint8_t* parity)
	{
		for (size_t i = 0; i < node_bytes; ++i)
		{
			symbol_t vals[k];
			symbol_t par[n - k];

			for (size_t j = 0; j < k; ++j)
			{
				vals[j] = data[i + j * node_bytes];
			}

			encode_single(vals, par);

			for (size_t j = 0; j < n - k; ++j)
			{
				parity[i + j * node_bytes] = par[j].value;
			}
		}
	}
}

