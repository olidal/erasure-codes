#include "galois.h"
#include "matrix.h"

#include <sstream>
#include <Windows.h>

using namespace erasure;

typedef galois_field<uint8_t> field;

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
