#pragma once

#include "symbol.h"

// Disable debug checks in ublas
// our type doesn't suffer from 
// precision issues anyway
#define BOOST_UBLAS_NDEBUG
// Disable exceptions in boost
// ublas methods
#define BOOST_NO_EXCEPTIONS

#include <boost/numeric/ublas/matrix.hpp>

namespace erasure
{
	typedef boost::numeric::ublas::matrix<symbol> matrix;

	bool inverse(matrix& mat);

	matrix vandermonde(size_t n, size_t k);
	matrix build_matrix(size_t n, size_t k);
}
