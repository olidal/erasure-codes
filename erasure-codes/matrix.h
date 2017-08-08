#pragma once

#include "symbol.h"

#include <boost/numeric/ublas/matrix.hpp>

namespace erasure
{
	typedef boost::numeric::ublas::matrix<symbol> matrix;

	bool inverse(matrix& mat);

	matrix vandermonde(size_t n, size_t k);
	matrix build_matrix(size_t n, size_t k);
}
