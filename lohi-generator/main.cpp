#include "symbol.h"

#include <sstream>

using namespace erasure;

int main(int argc, char** argv)
{
	std::stringstream ss;

	ss << "{\n" << std::hex;

	bool b1 = false, b2 = false;
	for (size_t i = 0; i < 256; ++i)
	{
		if (b1)
			ss << ",\n";
		else
			b1 = true;

		ss << "{ {";

		b2 = false;
		for (size_t j = 0; j < 16; ++j)
		{
			if (b2)
				ss << ',';
			else
				b2 = true;

			ss << "0x" << (size_t)mul((uint8_t)i, (uint8_t)j);
		}

		ss << "}, {";

		b2 = false;
		for (size_t j = 0; j < 16; ++j)
		{
			if (b2)
				ss << ',';
			else
				b2 = true;

			ss << "0x" << (size_t)mul((uint8_t)i, (uint8_t)j << 4);
		}

		ss << "} }";
	}

	ss << "\n}";

	printf("%s", ss.str().c_str());

	return 0;
}
