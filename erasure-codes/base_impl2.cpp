#include "interface.h"
#include "matrix.h"

using namespace erasure;

namespace base_impl2
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
	static matrix build_matrix(size_t n, size_t k)
	{
		matrix vm = vandermonde(n, k);

		return vm * inverse(vm.submatrix(k, k));
	}

	void encode(uint8_t** shards, uint8_t** parity)
	{

	}
	reconstruct_code reconstruct(uint8_t** shards, bool* present)
	{

	}
}
