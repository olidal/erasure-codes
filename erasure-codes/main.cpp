#include "galois.h"
#include "matrix.h"

#include <sstream>
#include <Windows.h>

using namespace erasure;

typedef galois_field<uint8_t> field;

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

int main()
{
	field::init();

	std::stringstream ss;
	ss << '{' << '\n';

	bool y = false;
	for (uint8_t i = 0;; i += 1)
	{
		if (y) ss << ',' << '\n';
		y = true;

		ss << '{' << '{';
		bool t = false;
		for (uint8_t j = 0;; j += 1)
		{
			if (t) ss << ',';
			t = true;

			ss << (uint32_t)(symbol_t(i) * symbol_t(j)).value;

			if (j == 15)
				break;
		}
		ss << "}, {";

		t = false;
		for (uint8_t j = 0;; j += 1)
		{
			if (t) ss << ',';
			t = true;

			ss << (uint32_t)(symbol_t(i) * symbol_t(j << 4)).value;

			if (j == 15)
				break;
		}

		ss << '}' << '}';

		if (i == 255)
			break;
	}
	ss << '}';

	printf(ss.str().c_str());
}
