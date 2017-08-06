#ifndef GALOIS_BUILD
#	define GALOIS_BUILD
#endif

#include "matrix.h"

#define BOOST_UBLAS_NDEBUG

#include <boost\numeric\ublas\matrix.hpp>
#include <boost\numeric\ublas\lu.hpp>
#include <boost\numeric\ublas\io.hpp>

namespace galois
{
	using namespace boost::numeric;

	int GALOIS_EXPORT invert_matrix(symbol *src, size_t k)
	{
		typedef ublas::permutation_matrix<std::size_t> pmatrix;

		ublas::matrix<symbol> A{ k, k }, inverse;
		pmatrix pm(A.size1());
		
		std::memcpy(A.data().begin(), src, sizeof(symbol) * k * k);

		size_t res = ublas::lu_factorize(A, pm);
		if (res != 0)
			return (int)res;

		inverse.assign(ublas::identity_matrix<symbol>(A.size1()));

		ublas::lu_substitute(A, pm, inverse);

		A.assign(inverse);

		std::memcpy(src, inverse.data().begin(), sizeof(symbol) * k * k);

		return 0;
	}
}
