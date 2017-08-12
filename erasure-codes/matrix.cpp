
#ifdef _MSC_VER
#	pragma warning(disable:4267 4996)
#endif

#include "matrix.h"

#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>

namespace erasure
{
	using boost::numeric::ublas::permutation_matrix;
	using boost::numeric::ublas::identity_matrix;

	using namespace boost::numeric;

	bool inverse(matrix& mat)
	{
		assert(mat.size1() == mat.size2());

		permutation_matrix<uint8_t> pm(mat.size1());

		if (ublas::lu_factorize(mat, pm) != 0)
			return false;

		matrix inverse = identity_matrix<symbol>(mat.size1());

		ublas::lu_substitute(mat, pm, inverse);

		mat.assign(inverse);

		return true;
	}

	matrix vandermonde(size_t n, size_t k)
	{
		matrix m{ n, k };
		
		for (uint8_t r = 0; r < n; ++r)
		{
			for (uint8_t c = 0; c < k; ++c)
			{
				m(r, c) = symbol::exp(r, c);
			}
		}

		return m;
	}
	matrix build_matrix(size_t n, size_t k)
	{
		auto vm = vandermonde(n, k);
		matrix inv = ublas::subrange(vm, 0, k, 0, k);

		auto r = inverse(inv);
		assert(r); // Matrix should never be singular

		return ublas::prod(vm, inv);
	}
}
